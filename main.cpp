/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "bugReport.hpp"
#include "log.hpp"

using namespace std;

class Object;

class Observer {
   public:
    Observer(string key, int value) : m_key(key), m_value(value) {}
    // 外部接口，通知到观察者的操作
    void update(Object *poj);

   private:
    string m_key;
    int m_value;
};


// 被观察者
class Object {
   public:
    int m_value;
    // 添加一位观察者
    void attach(string key, Observer *p_ob) {
        if (m_ob.find(key) != m_ob.end()) {
            LOG_WARN("%s has been attached", key.c_str());
            return;
        }
        m_ob[key] = p_ob;
        LOG_DEBUG("attach %s", key.c_str());
    }

    // 删除观察
    void deattach(string key) {
        auto it = m_ob.find(key);
        if (it != m_ob.end()) {
            m_ob.erase(it);
            LOG_DEBUG("erase %s", key);
        }
    }

    // 更新值
    void set_value(int value) {
        m_value = value;
        notify();
    }

   private:
    map<string, Observer *> m_ob;   // 观察者map
    // 通知所有观察者执行操作
    void notify() {
        for (auto it = m_ob.begin(); it != m_ob.end(); it++) {
            it->second->update(this);
        }
    }
};

void Observer::update(Object *poj) {
    if (m_value == poj->m_value) {
        LOG_DEBUG("%s, hhh", m_key.c_str());
    }
}

int main() {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    Object obj;
    Observer a("aObserver", 1);
    Observer b("bObserver", 2);
    Observer c("cObserver", 3);
    Observer d("dObserver", 4);

    // 观察者注册到被观察者
    obj.attach("a", &a);
    obj.attach("b", &b);
    obj.attach("c", &c);
    obj.attach("a", &a);

    while (true) {
        /* code */
        int value;
        cin >> value;
        obj.set_value(value);
    }

    return 0;
}
