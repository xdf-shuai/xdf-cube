#!/bin/sh
killall -9 envset_proc
rm -f instance/server/aspect_policy.cfg
rm -f instance/Student/aspect_policy.cfg
yes|cp instance/server/plugin_config.cfg.label instance/server/plugin_config.cfg
yes|cp instance/server/sys_config.cfg.label instance/server/sys_config.cfg
yes|cp instance/server/router_policy.cfg.record instance/server/router_policy.cfg
yes|cp instance/server/aspect_policy.cfg.label instance/server/aspect_policy.cfg

yes|cp instance/Student/sys_config.cfg.record instance/Student/sys_config.cfg
yes|cp instance/Student/router_policy.cfg.record instance/Student/router_policy.cfg
sleep 1

sh run_cube.sh exec_def/_user_server.def &
sleep 1
sh run_cube.sh exec_def/_Student_client.def login.msg write.msg read.msg
#sh run_cube.sh exec_def/_Student_client.def login.msg
