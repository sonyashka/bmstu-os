cmd_/home/sonyashka/sem6/os/lab_04/part_02/part_02/Module.symvers := sed 's/\.ko$$/\.o/' /home/sonyashka/sem6/os/lab_04/part_02/part_02/modules.order | scripts/mod/modpost -m -a  -o /home/sonyashka/sem6/os/lab_04/part_02/part_02/Module.symvers -e -i Module.symvers   -T -
