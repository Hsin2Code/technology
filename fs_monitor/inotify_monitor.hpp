#pragma once
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>

#include "monitor.hpp"
#include "fsm_container.hpp"
#include "fsm_event.hpp"
#include <sys/inotify.h>

namespace fsm {
    struct inotify_monitor_impl
    {
        // std::vector<event> events;
        int inotify_monitor_handle = -1;
        fsm_hash_set<int> watched_descriptors; // 被监控的描述符
        fsm_hash_map<std::string, int> path_to_wd;
        fsm_hash_map<int, std::string> wd_to_path;

        fsm_hash_set<int> descriptors_to_remove;
        fsm_hash_set<int> watches_to_remove;

        fsm_hash_set<std::string> paths_to_rescan; // 需要扫描的目录
        time_t curr_time;
    };

    class inotify_monitor : public monitor
    {
    public:
        inotify_monitor();
        virtual ~inotify_monitor();

    private:
        bool add_watch(const std::string &path);
        void scan(const std::string &path);
        void preprocess_event(inotify_event *event);
        // 在 inotify 监控期间做的处理
        void process_pending_events();

        inotify_monitor_impl *impl;
    };
    // 构造函数
    inotify_monitor::inotify_monitor() :
        impl(new inotify_monitor_impl())
    {
        impl->inotify_monitor_handle = inotify_init();

        if (impl->inotify_monitor_handle == -1)
        {
            // 异常
        }
    }
    // 析构函数
    inotify_monitor::~inotify_monitor()
    {
        if (impl->inotify_monitor_handle > 0) {
            close(impl->inotify_monitor_handle);
        }
        delete impl;
    }
    // 添加监控
    bool inotify_monitor::add_watch(const std::string &path)
    {
        int wd = inotify_add_watch(impl->inotify_monitor_handle,
                                   path.c_str(),
                                   IN_ALL_EVENTS);
        if (wd < 0) {
            perror("inotify_add_watch");
        }
        else {
            impl->watched_descriptors.insert(wd);
            impl->wd_to_path[wd] = path;
            impl->path_to_wd[path] = wd;
        }
        return (wd >= 0);
    }
    void inotify_monitor::scan(const std::string &path)
    {
        ;
    }
    // 预处理文件事件
    void inotify_monitor::preprocess_event(struct inotify_event *event)
    {
        if(event->mask & IN_Q_OVERFLOW) {
            // 溢出处理
        }
        // 目录事件
        {
            std::vector<fsm_event_flag> flags;
            if (event->mask & IN_ISDIR) flags.push_back(fsm_event_flag::IsDir);
            if (event->mask & IN_MOVE_SELF) flags.push_back(fsm_event_flag::Updated);
            if (event->mask & IN_UNMOUNT) flags.push_back(fsm_event_flag::PlatformSpecific);
            if (flags.size()) {
                // impl->events.push_back({impl->wd_to_path[event->wd], impl->curr_time, flags});
            }
            // 如果目录被创建监控目录以及其子目录
            if ((event->mask & IN_ISDIR) && (event->mask & IN_CREATE)) {
                impl->paths_to_rescan.insert(impl->wd_to_path[event->wd]);
            }
        }
        // 文件事件
        {
            std::vector<fsm_event_flag> flags;
            if (event->mask & IN_ACCESS) flags.push_back(fsm_event_flag::PlatformSpecific);
            if (event->mask & IN_ATTRIB) flags.push_back(fsm_event_flag::AttributeModified);
            if (event->mask & IN_CLOSE_NOWRITE) flags.push_back(fsm_event_flag::PlatformSpecific);
            if (event->mask & IN_CLOSE_WRITE) flags.push_back(fsm_event_flag::Updated);
            if (event->mask & IN_CREATE) flags.push_back(fsm_event_flag::Created);
            if (event->mask & IN_DELETE) flags.push_back(fsm_event_flag::Removed);
            if (event->mask & IN_MODIFY) flags.push_back(fsm_event_flag::Updated);
            if (event->mask & IN_MOVED_FROM) {
                flags.push_back(fsm_event_flag::Removed);
                flags.push_back(fsm_event_flag::MovedFrom);
            }
            if (event->mask & IN_MOVED_TO) {
                flags.push_back(fsm_event_flag::Created);
                flags.push_back(fsm_event_flag::MovedTo);
            }
            if (event->mask & IN_OPEN) flags.push_back(fsm_event_flag::PlatformSpecific);
            std::ostringstream filename_stream;
            filename_stream << impl->wd_to_path[event->wd];

            if (event->len > 1) {
                filename_stream << "/";
                filename_stream << event->name;
            }

            if (flags.size()) {
                // impl->events.push_back({filename_stream.str(), impl->curr_time, flags});
            }
            std::cout << "Generic event: " << event->wd
                      << "::" << filename_stream.str() << "\n";
            if (event->mask & IN_IGNORED) {
                std::cout << "IN_IGNORED: " << event->wd << "::" << filename_stream.str() << "\n";
                impl->descriptors_to_remove.insert(event->wd);
            }
            if (event->mask & IN_MOVE_SELF) {
                std::cout << "IN_MOVE_SELF: " << event->wd
                          << "::" << filename_stream.str() << "\n";
                impl->watches_to_remove.insert(event->wd);
                impl->descriptors_to_remove.insert(event->wd);
            }

            if (event->mask & IN_DELETE_SELF) {
                std::cout << "IN_DELETE_SELF: " << event->wd
                          << "::" << filename_stream.str() << "\n";

                impl->descriptors_to_remove.insert(event->wd);
            }
        }
    }
    void inotify_monitor::process_pending_events() {
        // 移除监视
        for (auto &wd : impl->watches_to_remove) {
            if(inotify_rm_watch(impl->inotify_monitor_handle, wd) != 0) {
                perror("inotify_rm_watch");
            }
            else {
                // LOG
            }
        }
        impl->watches_to_remove.clear();
        // 清理陈旧描述
        for (auto &wd : impl->descriptors_to_remove) {
            const std::string & curr_path = impl->wd_to_path[wd];
            impl->path_to_wd.erase(curr_path);
            impl->wd_to_path.erase(wd);
        }
        impl->descriptors_to_remove.clear();
        // 扫描新增目录
        std::for_each(impl->paths_to_rescan.begin(),
                      impl->paths_to_rescan.end(),
                      [this] (const std::string &p) {
                          this->scan(p);
                      } );
        impl->paths_to_rescan.clear();
    }
}  // fsm
