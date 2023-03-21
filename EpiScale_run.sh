#!/bin/csh

#$ -M blank@nd.edu	 # Email address for job notification
#$ -m  abe		 # Send mail when job begins, ends and aborts 
#$ -q  *@@acms_gpu 	 # Specify queue
#s -pe smp 4 
#$ -N  run_test	 # Specify job name


module load slurm/17.02.5
module load cmake
module load cuda/11.4
module load gcc/6.3.0
#module load extra
#module load openmpi/2.0.1-slurm-16.05.4
echo -n "It is currently: ";date
echo -n "I am logged on as ";who am i
echo -n "This computer is called ";hostname
echo -n "I am currently in the directory ";pwd
#setenv PATH /afs/crc.nd.edu/user/a/anematba/Public/2015/Oct/11th/SceCells/bin:$PATH
./bin/runDiscSimulation_M -slurm N01_0
