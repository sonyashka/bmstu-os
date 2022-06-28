cmd_/home/sonyashka/sem6/os/lab_08/queue/modules.order := {   echo /home/sonyashka/sem6/os/lab_08/queue/my_workqueue.ko; :; } | awk '!x[$$0]++' - > /home/sonyashka/sem6/os/lab_08/queue/modules.order
