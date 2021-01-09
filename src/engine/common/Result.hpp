#ifndef ASTRO_RESULT_HPP
    #define ASTRO_RESULT_HPP

    #include "Type.hpp"

    namespace astro {
        
        namespace ResultType {
            enum ResultType : int {
                noop = 0,
                Success,
                Failure,
                Waiting
            };
            static std::string name(int type){
                switch(type){
                    case ResultType::Success:
                        return "Success";
                    case ResultType::Failure:
                        return "Failure";
                    case ResultType::Waiting:
                        return "Waiting";
                    default:
                        return "Undefined";
                }
            }
        }

        struct  Result {
            int val;
            std::string msg;
            void *payload;
            size_t psize; // payload size (in bytes)
            Result(){
                this->val = ResultType::noop;
                this->msg = "";
                this->payload = NULL;
                this->psize = 0;
            }
            Result(int val, const std::string &msg, void *payload){
                set(val, msg, payload);
            }
            Result(int val, const std::string &msg){
                set(val, msg, NULL);
            }            
            Result(int val){
                set(val, "", NULL);
            }
            Result(int val, void *payload){
                set(val, "", payload);
            }            
            void set(int val, const std::string &msg, void *payload){
                this->val = val;
                this->msg = msg;
                this->payload = payload;
            }
            void set(int val, const std::string &msg = ""){
                this->val = val;
                this->msg = msg;
                this->payload = NULL;
            }            
            std::string str() const {
                return ResultType::name(val) + (this->msg.length() > 0 ? " "+this->msg : "");
            }
            operator std::string() const {
                return str();
            };
            operator bool() const {
                return val == ResultType::Success;
            }
        };

    }

#endif