/**
 * @file   zkclient.h
 * @author liuyuan <yuan.liu1@renren-inc.com>
 * @author Zhe Yuan <zhe.yuan@renren-inc.com>
 * @date   Mon Aug 26 22:30:27 2013
 * 
 * @brief  
 * 
 * 
 */
#ifndef XCS_CCZK_ZKCLIENT_H_
#define XCS_CCZK_ZKCLIENT_H_

#include <zookeeper/zookeeper.h>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/concept_check.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <set>
#include <string>
#include <vector>

#include <zookeeper_config.h>
#include <zookeeper_const.h>
#include <watcher.h>

namespace xcs {
namespace cczk {
  using boost::noncopyable;
  class ZkClient : noncopyable {
   private :
    //  <watch_data, watch_child>
    typedef std::pair<bool, bool> PreprotyOfPath;
    //  watcher
    typedef boost::shared_ptr<watcher> ListenerMapKey;
    //  <path, preproty>
    typedef std::map<string, PreprotyOfPath> ListenerMapValue;
    typedef std::map<ListenerMapKey, ListenerMapValue> ListenerMap;
    zhandle_t *zhandle_;
    boost::mutex _glob_mutex;
    zookeeper_config config_;
    boost::thread background_watcher_thread_;
    bool background_watcher_;
    boost::recursive_mutex background_mutex_;
    ListenerMap listeners_;
    boost::mutex singleton_mutex_;
    ZkClient();
    void UpdateAuth();
    zhandle_t* CreateConnection();
    static void init_watcher(zhandle_t *zh, int type,
        int state, const char *path, void *watcherCtx);
    static void event_watcher(zhandle_t *zh, int type,
        int state, const char *path, void *watcherCtx);
    void WatcherLoop();
    ~ZkClient();
    void TriggerAllWatcher(ListenerMap &);
   public  :
    static ZkClient* Open(const zookeeper_config *config = NULL);
    void Close();
    void Clear();
    bool IsAvailable();
    ReturnCode::type GetChildrenOfPath(const string/*path*/,
                                          std::vector<string>&/*children*/);
    ReturnCode::type SetDataOfNode(const string/*path*/,
                                      string&/*value*/);
    ReturnCode::type GetDataOfNode(const string/*path*/,
                                      string&/*value*/);
    ReturnCode::type CreateNode(const string/*path*/,
                                 const string&/*value*/,
                                 CreateMode::type/*mode*/);
    ReturnCode::type DeleteNode(const string/*path*/);
    ReturnCode::type Exist(const string/*path*/);
    ReturnCode::type AddListener(boost::shared_ptr<watcher>/*listener*/,
                                  string/*path*/);
    ReturnCode::type DropListener(boost::shared_ptr<watcher>/*listener*/);
    ReturnCode::type DropListenerWithPath(boost::shared_ptr<watcher>/*listener*/,
                                             string/*path*/);
  };
}  // namespace cczk
}  // namespace xcs

#endif  // XCS_CCZK_ZKCLIENT_H_