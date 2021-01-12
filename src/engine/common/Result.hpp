#ifndef ASTRO_RESULT_HPP
    #define ASTRO_RESULT_HPP

    #include <memory>
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

        struct Job;
        struct SmallPacket;
        struct  Result {
            std::shared_ptr<Result> self;
            bool done;
            int val;
            std::string msg;
            std::shared_ptr<astro::SmallPacket> payload;
            std::shared_ptr<astro::Job> job;
            std::function<void(const std::shared_ptr<astro::Result> &result)> onSuccess;
            std::function<void(const std::shared_ptr<astro::Result> &result)> onFailure;
            void *ref;
            Result(const astro::Result &result);
            Result();
            Result(int val, const std::string &msg);
            Result(int val, const std::shared_ptr<astro::SmallPacket> &payload);
            Result(int val, const std::shared_ptr<astro::Job> &job);                          
            Result(int val);
            void set(int val, const std::string &msg = "");
            void set(void *ref);
            void set(const std::shared_ptr<astro::Job> &job);
            void set(const std::shared_ptr<astro::SmallPacket> &payload);
            void setFailure(const std::string &msg = "");
            void setSuccess(const std::string &msg = "");
            void initDefault();
            void setOnSuccess(const std::function<void(const std::shared_ptr<astro::Result> &result)> &lambda);
            void setOnFailure(const std::function<void(const std::shared_ptr<astro::Result> &result)> &lambda);
            
            bool isSuccessful();      
            std::string str() const;
            operator std::string() const;
            operator bool() const;
        };

        std::shared_ptr<astro::Result> makeResult();

        std::shared_ptr<astro::Result> makeResult(int val, const std::string &msg = "");
        std::shared_ptr<astro::Result> makeResult(int val, const std::shared_ptr<astro::Job> &job);

        std::shared_ptr<astro::Result> makeResult(int val, const std::shared_ptr<astro::SmallPacket> &payload);

    }

#endif