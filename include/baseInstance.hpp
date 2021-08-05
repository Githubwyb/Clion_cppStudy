/**
 * @file baseInstance.hpp
 * @author wangyubo (18433@sangfor.com)
 * @brief 线程安全的单例库
 * @version 0.1
 * @date 2021-07-21
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef BASEINSTANCE_HPP
#define BASEINSTANCE_HPP

class BaseNoCopy {
   public:
    BaseNoCopy() {}

   private:
    BaseNoCopy(const BaseNoCopy &obj);
    BaseNoCopy(BaseNoCopy *pObj);
    BaseNoCopy(const BaseNoCopy *pObj);
    BaseNoCopy &operator=(const BaseNoCopy &obj);
    BaseNoCopy &operator=(const BaseNoCopy *pObj);
};

template <class T>
class BaseInstance : public BaseNoCopy {
   public:
    static T &getInstance() {
        static T s_instance;
        CreateObject.do_nothing();

        return s_instance;
    }

   protected:
    BaseInstance() {}

   private:
    BaseInstance(const BaseInstance &obj);
    BaseInstance operator=(const BaseInstance &obj);

    struct ObjectCreator {
        ObjectCreator() { BaseInstance<T>::getInstance(); }

        inline void do_nothing() const {}
    };
    static ObjectCreator CreateObject;
};

template <typename T>
typename BaseInstance<T>::ObjectCreator BaseInstance<T>::CreateObject;

#endif
