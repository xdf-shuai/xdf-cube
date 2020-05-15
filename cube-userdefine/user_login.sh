#!/bin/sh
killall -9 envset_proc
rm -f instance/server/aspect_policy.cfg
rm -f instance/Student/aspect_policy.cfg
yes|cp instance/server/plugin_config.cfg.user instance/server/plugin_config.cfg
yes|cp instance/server/sys_config.cfg.user instance/server/sys_config.cfg
yes|cp instance/server/router_policy.cfg.user instance/server/router_policy.cfg

yes|cp instance/Student/sys_config.cfg.user instance/Student/sys_config.cfg
yes|cp instance/Student/router_policy.cfg.user instance/Student/router_policy.cfg
sleep 1

sh run_cube.sh exec_def/_user_server.def &
sleep 1
sh run_cube.sh exec_def/_Student_client.def login.msg
