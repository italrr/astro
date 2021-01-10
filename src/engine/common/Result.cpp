#include "Result.hpp"
#include "Job.hpp"
#include "Tools.hpp"

astro::Result::Result(const astro::Result &result){
    this->val = result.val;
    this->msg = result.msg;
    this->payload = result.payload;
    this->job = result.job;
    this->done = result.done;
}

astro::Result::Result(){
    this->val = ResultType::noop;
    this->msg = "";
    this->job = std::shared_ptr<astro::Job>(NULL);
    this->payload = std::shared_ptr<astro::SmallPacket>(NULL);
    this->done = false;
}

astro::Result::Result(int val, const std::string &msg){
    set(val, msg);
    this->job = std::shared_ptr<astro::Job>(NULL);
    this->done = val != ResultType::Waiting;
}  

astro::Result::Result(int val, const std::shared_ptr<astro::SmallPacket> &payload){
    set(val, "");
    this->job = std::shared_ptr<astro::Job>(NULL);
    this->payload = payload;
    this->done = val != ResultType::Waiting;
}  

astro::Result::Result(int val, const std::shared_ptr<astro::Job> &job){
    set(val, "");
    this->job = job;
    this->payload = std::shared_ptr<astro::SmallPacket>(NULL);
    this->done = val != ResultType::Waiting;
}   

astro::Result::Result(int val){
    set(val, "");
    this->job = std::shared_ptr<astro::Job>(NULL);
    this->done = val != ResultType::Waiting;
}   

void astro::Result::set(int val, const std::string &msg){
    this->val = val;
    this->msg = msg;
    this->done = val != ResultType::Waiting;
}    

void astro::Result::set(void *ref){
    this->ref = ref;
}

void astro::Result::set(const std::shared_ptr<astro::Job> &job){
    this->job = job;
}

void astro::Result::set(const std::shared_ptr<astro::SmallPacket> &payload){
    this->payload = payload;
}

void astro::Result::setFailure(const std::string &msg){
    this->val = ResultType::Failure;
    this->msg = msg;
    this->done = true;
}

void astro::Result::setSuccess(const std::string &msg){
    this->val = ResultType::Success;
    this->msg = msg;
    this->done = true;
}   

bool astro::Result::isSuccessful(){
    return this->val == ResultType::Success;
}         

std::string astro::Result::str() const {
    return ResultType::name(val) + (this->msg.length() > 0 ? " "+this->msg : "");
}

astro::Result::operator std::string() const {
    return str();
}

astro::Result::operator bool() const {
    return val == ResultType::Success;
}


std::shared_ptr<astro::Result> astro::makeResult(){
    return std::make_shared<astro::Result>(astro::Result());
}

std::shared_ptr<astro::Result> astro::makeResult(int val, const std::string &msg){
    return std::make_shared<astro::Result>(astro::Result(val, msg));
}

std::shared_ptr<astro::Result> astro::makeResult(int val, const std::shared_ptr<astro::Job> &job){
    auto r = std::make_shared<astro::Result>(astro::Result(val, ""));
    r->job = job;
    return r;
}     

std::shared_ptr<astro::Result> astro::makeResult(int val, const std::shared_ptr<astro::SmallPacket> &payload){
    auto r = std::make_shared<astro::Result>(astro::Result(val, ""));
    r->payload= payload;
    return r;
}                