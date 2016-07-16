// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
// C++ Compiler Used: GNU, Intel
// Produced By: DataReel Software Development Team
// File Creation Date: 06/17/2016
// Date Last Modified: 07/16/2016
// Copyright (c) 2016 DataReel Software Development
// ----------------------------------------------------------- // 
// ---------- Include File Description and Details  ---------- // 
// ----------------------------------------------------------- // 
/*
This file is part of the DataReel software distribution.

Datareel is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version. 

Datareel software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with the DataReel software distribution.  If not, see
<http://www.gnu.org/licenses/>.

Program config funcions for Datareel load balancer.

*/
// ----------------------------------------------------------- //   
#ifndef __DRLB_CONFIG_HPP__
#define __DRLB_CONFIG_HPP__

#include "gxdlcode.h"
#include "drlb_server.h"

enum LBschemes {
  LB_NONE = 0, // NONE value used with LB_ASSIGNED rules
  LB_ASSIGNED,
  LB_DISTRIB,
  LB_RR,
  LB_WEIGHTED
};

enum LB_rule_type {
  LB_ROUTE = 0
};

struct LB_rule {
  LB_rule() {
    rule =  LB_ROUTE;
  }

  gxString node_name;
  gxString front_end_client_ip;
  LB_rule_type rule;
};

// Queue node used for log file and console messages
struct LogQueueNode {

  LogQueueNode() {
    memset(sbuf, 0, 255);
    dirty[0] = 0;
  }

  char sbuf[255];
  char dirty[1];
};

// Load Balancer node type
struct LBnode {
  LBnode();
  LBnode(const LBnode &n) { Copy(n); }
  LBnode operator=(const LBnode &n) {
    Copy(n);
    return *this;
  }
  void Copy(const LBnode &n);

  // Overloads for load percent and weight calculations
  friend int operator==(const LBnode &a, const LBnode &b);
  friend int operator!=(const LBnode &a, const LBnode &b);
  friend int operator<(const LBnode &a, const LBnode &b);
  friend int operator>(const LBnode &a, const LBnode &b);

  int LB_FLAG(int flag = -1);
  unsigned NUM_CONNECTIONS(int inc = 0, int dec = 0, unsigned set_to = 0); 

  gxString node_name;    // String name for this node
  gxsPort_t port_number; // Port number for service we are load balancing
  gxString hostname;     // IP address or hostname for host providing service
  int weight;            // Weight or load percent for this host
  LBschemes scheme;      // Type of load balancing
  int max_connections;   // Max concurrent connections for this node
  int buffer_size;       // read buffer size  
  LBnode *active_ptr;    // Pointer used by a copy of this object to reference the active node

private: // Cannot be accessed directly
  int num_connections;
  int lb_flag;

private: // Internal thread vars
  gxMutex num_connections_lock;
  gxCondition num_connections_cond;
  int num_connections_is_locked;
  int num_connections_retries;
  gxMutex lb_flag_lock;
  gxCondition lb_flag_cond;
  int lb_flag_is_locked;
  int lb_flag_retries;
};

// Node type used to refactor distributed nodes
struct limit_node {
  limit_node() {
    limit = 1;
    active_ptr = 0;
  }
  
  limit_node(const limit_node &n) { Copy(n); }
  limit_node operator=(const limit_node &n) {
    Copy(n);
    return *this;
    }
  void Copy(const limit_node &n) {
    limit = n.limit;
    active_ptr = n.active_ptr;
  }
  int limit;
  LBnode *active_ptr;
};

struct LBconfig {
  LBconfig();
  ~LBconfig();

  unsigned NUM_NODES(); // Get the number of LB nodes
  unsigned NUM_CLIENT_THREADS(int inc = 0, int dec = 0, unsigned set_to = 0); 
  int QUEUE_NODE_COUNT();
  int QUEUE_DEBUG_COUNT();
  int QUEUE_PROC_COUNT();
  int ReadRulesConfig();

  LBschemes scheme;  // Type of load balancing
  gxList<LBnode *> nodes;
  int use_buffer_cache; // Global buffer cache setting
  int max_threads;     // Max threads for all nodes 
  unsigned buffer_size;
  int debug;
  int debug_level;
  int verbose;
  int verbose_level;
  gxString config_file;
  gxList<LB_rule *> rules_config_list;
  gxString rules_config_file;
  int dynamic_rules_read;
  LBschemes assigned_default;
  gxString service_name;
  gxString ProgramName;
  int user_config_file;
  gxsPort_t port;     // Server's port number
  int accept_clients; // Ture while accepting 
  int echo_loop;      // True to keep all connection persistent
  thrPool *client_request_pool; // Worker threads processing client requests
  LogFile logfile; // Log file object  
  int fatal_error;
  gxThread_t *server_thread;
  gxThread_t *log_thread;
  gxThread_t *console_thread;
  unsigned buffer_overflow_size;
  int enable_buffer_overflow_detection;
  unsigned retries;
  unsigned retry_wait;
  int timeout_secs;
  int timeout_usecs;
  int use_timeout;
  int somaxconn;
  gxsSocket_t server_accept_socket;
  int server_retry;
  int max_connections;   // Max concurrent connections for this server

  // Log settings
  int clear_log;
  int enable_logging;
  int num_logs_to_keep;
  long max_log_size;
  int last_log;
  gxString logfile_name;
  int log_level;
  int log_queue_size;
  int log_queue_debug_size;
  int log_queue_proc_size;
  LogQueueNode *loq_queue_nodes;
  LogQueueNode *loq_queue_debug_nodes;
  LogQueueNode *loq_queue_proc_nodes;

  gxList<LBnode *> distributed_rr_node_list;
  gxList<LBnode *> weighted_rr_node_list;
  int refactor_scale;
  int build_distributed_rr_node_list();
  int refactor_distribution_limits(gxList<limit_node> &limit_node_list, int num_connections);
  int build_weighted_rr_node_list();
  int refactor_weighted_limits(gxList<limit_node> &limit_node_list, int scale);
  int get_num_server_connections();
  int check_node_max_clients(LBnode *n, int seq_num);
  int WEIGHT_SCALE(int inc = 0, int dec = 0, unsigned set_to = 0);
  int PREV_REFACTORED_CONNECTIONS(int inc = 0, int dec = 0, unsigned set_to = 0);
  int REFACTORED_CONNECTIONS(int inc = 0, int dec = 0, unsigned set_to = 0);

private: // Cannot be accessed directly
  int refactored_connections;
  int prev_refactored_connections; 
  int weight_scale;
  int prev_refactored_connections_is_locked;
  int prev_refactored_connections_retries;
  unsigned num_nodes;
  int num_client_threads;
  int queue_node_count;
  int queue_debug_count;
  int queue_proc_count;

private: // Internal thread vars
  // Num nodes
  gxMutex num_nodes_lock;
  gxCondition num_nodes_cond;
  int num_nodes_is_locked;
  int num_nodes_thread_retries;

  // Num client threads
  gxMutex num_client_threads_lock;
  gxCondition num_client_threads_cond;
  int num_client_threads_is_locked;
  int num_client_threads_retries;

  // Assgined config
  gxMutex rules_config_lock;
  gxCondition rules_config_cond;
  int rules_config_is_locked;
  int rules_config_retries;

  // Log queue
  gxMutex queue_node_count_lock;
  gxCondition queue_node_count_cond;
  int queue_node_count_is_locked;
  int queue_node_count_retries;
  gxMutex queue_debug_count_lock;
  gxCondition queue_debug_count_cond;
  int queue_debug_count_is_locked;
  int queue_debug_count_retries;
  gxMutex queue_proc_count_lock;
  gxCondition queue_proc_count_cond;
  int queue_proc_count_is_locked;
  int queue_proc_count_retries;

  gxMutex weight_scale_lock;
  gxCondition weight_scale_cond;
  int weight_scale_is_locked;
  int weight_scale_retries;
  gxMutex refactored_connections_lock;
  gxCondition refactored_connections_cond;
  int refactored_connections_is_locked;
  int refactored_connections_retries;
  gxMutex prev_refactored_connections_lock;
  gxCondition prev_refactored_connections_cond;
};

// Config functions
int LoadOrBuildConfigFile();
int ProcessArgs(int argc, char *argv[]);
int ProcessDashDashArg(gxString &arg);

extern LBconfig ServerConfigSruct;
extern LBconfig *servercfg;

#endif // __DRLB_CONFIG_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
