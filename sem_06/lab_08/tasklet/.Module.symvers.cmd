cmd_/home/sonyashka/sem6/os/lab_08/tasklet/Module.symvers := sed 's/\.ko$$/\.o/' /home/sonyashka/sem6/os/lab_08/tasklet/modules.order | scripts/mod/modpost -m -a  -o /home/sonyashka/sem6/os/lab_08/tasklet/Module.symvers -e -i Module.symvers   -T -