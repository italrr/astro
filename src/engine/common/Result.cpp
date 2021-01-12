#include "Result.hpp"
#include "Job.hpp"
#include "Tools.hpp"

astro::Result::Result(const astro::Result &result){
    initDefault();
    this->val = result.val;
    this->msg = result.msg;
    this->payload = result.payload;
    this->job = result.job;
    this->done = result.done;
}

astro::Result::Result(){
    initDefault();    
    this->val = ResultType::noop;
    this->msg = "";
    this->job = std::shared_ptr<astro::Job>(NULL);
    this->payload = std::make_shared<astro::SmallPacket>(SmallPacket());
    this->done = false;
}

astro::Result::Result(int val, const std::string &msg){
    initDefault();
    this->val = val;
    this->msg = msg;
    this->done = val != ResultType::Waiting;
    this->job = std::shared_ptr<astro::Job>(NULL);
    this->done = val != ResultType::Waiting;
    this->payload = std::make_shared<astro::SmallPacket>(SmallPacket());
}  

astro::Result::Result(int val, const std::shared_ptr<astro::SmallPacket> &payload){
    initDefault();
    this->val = val;
    this->msg = msg;
    this->done = val != ResultType::Waiting;
    this->job = std::shared_ptr<astro::Job>(NULL);
    this->payload = payload;
    this->done = val != ResultType::Waiting;
}  

astro::Result::Result(int val, const std::shared_ptr<astro::Job> &job){
    initDefault();
    this->val = val;
    this->msg = msg;
    this->done = val != ResultType::Waiting;
    this->job = job;
    this->payload = std::shared_ptr<astro::SmallPacket>(NULL);
    this->done = val != ResultType::Waiting;
    this->payload = std::make_shared<astro::SmallPacket>(SmallPacket());
}   

astro::Result::Result(int val){
    initDefault();
    this->val = val;
    this->msg = msg;
    this->done = val != ResultType::Waiting;
    this->job = std::shared_ptr<astro::Job>(NULL);
    this->done = val != ResultType::Waiting;
}   

void astro::Result::set(int val, const std::string &msg){
    this->val = val;
    this->msg = msg;
    this->done = val != ResultType::Waiting;
    switch(val){
        case ResultType::Success: {
            this->onSuccess(self);
        } break;
        case ResultType::Failure: {
            this->onFailure(self);
        } break;        
    }
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
    this->onFailure(self);
}

void astro::Result::setSuccess(const std::string &msg){
    this->val = ResultType::Success;
    this->msg = msg;
    this->done = true;
    this->onSuccess(self);
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


void astro::Result::initDefault(){
    onSuccess = [](const std::shared_ptr<astro::Result> &lambda){
        return;
    };
    onFailure = [](const std::shared_ptr<astro::Result> &lambda){
        return;
    };    
}

void astro::Result::setOnSuccess(const std::function<void(const std::shared_ptr<astro::Result> &result)> &lambda){
    this->onSuccess = lambda;
}

void astro::Result::setOnFailure(const std::function<void(const std::shared_ptr<astro::Result> &result)> &lambda){
    this->onFailure = lambda;
}

std::shared_ptr<astro::Result> astro::makeResult(){
    auto result = std::make_shared<astro::Result>(astro::Result());
    result->self = result;
    return result;
}

std::shared_ptr<astro::Result> astro::makeResult(int val, const std::string &msg){
    auto result = std::make_shared<astro::Result>(astro::Result(val, msg));
    result->self = result;
    return result;
}

std::shared_ptr<astro::Result> astro::makeResult(int val, const std::shared_ptr<astro::Job> &job){
    auto r = std::make_shared<astro::Result>(astro::Result(val, ""));
    r->self = r;
    r->job = job;
    return r;
}     

std::shared_ptr<astro::Result> astro::makeResult(int val, const std::shared_ptr<astro::SmallPacket> &payload){
    auto r = std::make_shared<astro::Result>(astro::Result(val, ""));
    r->payload = payload;
    r->self = r;
    return r;
}                
