#include <chrono>
#include <thread>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <random>
#include <mutex>


#include "Tools.hpp"
#include "3rdparty/MD5.hpp"
#include "../Core.hpp"

/*
	TIME
*/

uint64 astro::ticks(){
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

uint64 astro::epoch(){
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void astro::sleep(uint64 t){
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}

/*
	HASH
*/
namespace astro{
	namespace Hash {
        std::string md5(const std::string &path){
			if(!File::exists(path)){
				return "";
			}
			
			FILE *f = fopen(path.c_str(), "rb");
			fseek(f, 0, SEEK_END);
			size_t fsize = ftell(f);
			fseek(f, 0, SEEK_SET);
			char *buffer = (char*)malloc(fsize);
			fread(buffer, fsize, 1, f);
			fclose(f);
			std::string hash = ThirdPartyMD5::md5(buffer, fsize);
			free(buffer);
			return hash;
		}

        std::string md5(char *data, size_t size){
			return ThirdPartyMD5::md5(data, size);
		}		
	}
}

/* 
	FILE
*/
namespace astro {
	namespace File {

		std::string dirSep(){
			return Core::PLATFORM == Core::SupportedPlatform::LINUX ? "/" : "\\";
		}

		bool exists(const std::string &path){
			struct stat tt;
			stat(path.c_str(), &tt);
			return S_ISREG(tt.st_mode);		
		}

		size_t size(const std::string &path){
			if (!exists(path)){
				return 0;
			}
			struct stat tt;
			stat(path.c_str(), &tt);
			return tt.st_size;			
		}

		std::string format(const std::string &filename){
			int k = filename.rfind(".");
			return k != -1 ? filename.substr(k+1, filename.length()-1) : "";
		}

		std::string filename(const std::string &path){
			int k = path.rfind("/");
			return k != -1 ? path.substr(k+1, path.length()-1) : path;			
		}

		astro::Result list(const std::string &path, const std::string &format, int type, bool recursively, std::vector<std::string> &output){
			astro::Result result(astro::ResultType::noop);
			DIR *directory;
			struct dirent *ent;
			std::vector<std::string> formats = String::split(format, "|");
			bool anyf = !((int)formats.size());
			directory = opendir(path.c_str());
			if (directory == NULL){
				result.set(ResultType::Failure, "directory '"+path+"' does not exist");
				return result;
			}
			std::string filename;
			while ((ent = readdir(directory))) {
				filename = ent->d_name;
				if (filename == "." || filename == ".."){
					continue;
				}
				std::string fpath = path + dirSep() + filename;
				struct stat ft;
				stat(fpath.c_str(), &ft);
				/* Directory */
				if (S_ISDIR(ft.st_mode)) {
					if (type == ListType::Directory || anyf){
						output.push_back(fpath);
					}
					if (recursively){
						File::list(fpath, format, type, recursively, output);
					}
				}else
				/* File */
				if (S_ISREG(ft.st_mode)){
					if (type != ListType::File && type != ListType::Any){
						continue;
					}
					bool add = anyf;
					if (!anyf){
						for (int i = 0; i< formats.size(); ++i){
							if (File::format(fpath) == formats[i]){
								add = true;
								break;
							}
						}
					}
					if (add){
						output.push_back(fpath);
					}					
				}
			}
			closedir (directory);
			result.set(output.size() > 0 ? ResultType::Success : ResultType::Failure);
			return result;
		}

	}
}

/*
	STRING
*/
std::string astro::String::toLower(const std::string &str){
	std::string out;
	for(int i = 0; i < str.size(); i++){
		out += tolower(str.at(i));
	}
	return out;
}
std::vector<std::string> astro::String::split(const std::string &str, const std::string &sep){
    std::vector<std::string> tokens;
    char *token = strtok((char*)str.c_str(), sep.c_str()); 
    while(token != NULL){ 
        tokens.push_back(std::string(token));
        token = strtok(NULL, "-"); 
    } 
    return tokens;
}

std::string astro::String::format(const std::string &_str, ...){
	char buffer[1024];
    va_list arg;
    va_start (arg, _str);
	sprintf (buffer, _str.c_str(), arg);
    va_end (arg);
	return std::string(buffer);	
}

/* 
	MATH
*/
namespace astro {
	namespace Math {
		int random(int min, int max){
			static std::random_device rd;
			std::mt19937 rng(rd());
			std::uniform_int_distribution<int> uni(min,max);
			return uni(rng);			
		}
	}
}

/* 
	SMALLPACKET
*/
namespace astro {

	SmallPacket::SmallPacket(const astro::SmallPacket &other){
		copy(other);
	}

	SmallPacket::SmallPacket(){
		this->data = NULL;
		clear();
	}

	SmallPacket::~SmallPacket(){
		std::unique_lock<std::mutex> lk(accesMutex);
		if(this->data != NULL){
			delete this->data;
		}
		lk.unlock();
	}

	void SmallPacket::copy(const SmallPacket &other){
		std::unique_lock<std::mutex> lk(accesMutex);
		if(this->data == NULL){
			this->data = (char*)malloc(ASTRO_SMALLPACKET_SIZE);
		}
		if(other.data == NULL){
			lk.unlock();
			clear();
			return;
		}
		memcpy(this->data, other.data, ASTRO_SMALLPACKET_SIZE);
		lk.unlock();
		reset();
	}

	SmallPacket& SmallPacket::operator= (const SmallPacket &other){
		copy(other);
		return *this;
	}

	void SmallPacket::clear(){
		std::unique_lock<std::mutex> lk(accesMutex);
		if(this->data != NULL){
			this->data = (char*)malloc(ASTRO_SMALLPACKET_SIZE);
		}
		index = 0; 
		lk.unlock();
	}

	void SmallPacket::reset(){
		std::unique_lock<std::mutex> lk(accesMutex);
		index = 0; 
		lk.unlock();
	}

	void SmallPacket::setIndex(size_t index){
		std::unique_lock<std::mutex> lk(accesMutex);
		this->index = index;
		lk.unlock();
	}

	std::shared_ptr<astro::Result> SmallPacket::read(std::string &str){
		std::unique_lock<std::mutex> lk(accesMutex);
		auto r = astro::makeResult(astro::ResultType::Success);
		if(this->data == NULL){
			this->data = (char*)malloc(ASTRO_SMALLPACKET_SIZE);
		}	
		if(index >= ASTRO_SMALLPACKET_SIZE){
			r->setFailure("packet full");
			lk.unlock();
			return r;
		}
		for(size_t i = index; i < ASTRO_SMALLPACKET_SIZE; ++i){
			if (this->data[i] == '\0'){
				size_t size = i - index; // don't include the nullterminated
				char buff[size];
				memcpy(buff, data + index, size);
				str = std::string(buff, size);
				index = i + 1;
				lk.unlock();
				return r;
			}
		}
		lk.unlock();
		return r;
	}

	std::shared_ptr<astro::Result> SmallPacket::read(void *data, size_t size){
		std::unique_lock<std::mutex> lk(accesMutex);
		auto r = astro::makeResult(astro::ResultType::Success);
		if(this->data == NULL){
			this->data = (char*)malloc(ASTRO_SMALLPACKET_SIZE);
		}	
		if((index >= ASTRO_SMALLPACKET_SIZE) || (index + size > ASTRO_SMALLPACKET_SIZE)){
			r->setFailure("packet full or won't fit");
			lk.unlock();
			return r;
		}
		memcpy(data, this->data + index, size);
		index += size;
		lk.unlock();
		return r;
	}		

	std::shared_ptr<astro::Result> SmallPacket::write(const std::string str){
		std::unique_lock<std::mutex> lk(accesMutex);
		auto r = astro::makeResult(astro::ResultType::Success);
		if(this->data == NULL){
			this->data = (char*)malloc(ASTRO_SMALLPACKET_SIZE);
		}	
		if((index >= ASTRO_SMALLPACKET_SIZE) || (index + str.length() + 1 > ASTRO_SMALLPACKET_SIZE)){
			r->setFailure("packet full or won't fit");
			lk.unlock();
			return r;
		}
		size_t sl = str.length() + 1;
		memcpy(data + index, str.c_str(), sl);
		index += sl;
		lk.unlock();
		return r;
	}

	std::shared_ptr<astro::Result> SmallPacket::write(const void *data, size_t size){
		std::unique_lock<std::mutex> lk(accesMutex);
		auto r = astro::makeResult(astro::ResultType::Success);
		if(this->data == NULL){
			this->data = (char*)malloc(ASTRO_SMALLPACKET_SIZE);
		}	
		if((index >= ASTRO_SMALLPACKET_SIZE) || (index + size > ASTRO_SMALLPACKET_SIZE)){
			r->setFailure("packet full or won't fit");
			lk.unlock();
			return r;
		}
		memcpy(this->data + index, data, size);
		index += size;
		lk.unlock();
		return r;
	}
}