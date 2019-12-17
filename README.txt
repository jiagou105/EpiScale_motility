CUDA code for developmental biology using Subcellular Element Method

Hardware requirement: 
Nvidia video card that supports SM 2.0+ and CUDA 4.0 

Software environment requirement: 
CMAKE ----- Build system.
CUDA  ----- Provide runtime support for parallel GPU computation.
Thrust ---- Build-in library of cuda, similar to STL of C++
Paraview -- (Optional) Visualization software for animation purpose. 


Location of configuration files:
 ./resources
*******************************************
To build code on UCR HPCC clusters "cluster.hpcc.ucr.edu":
   (1) git clone https://github.com/AliNemat/EpiScale_Signal.git
   (2) login to an available gpu: srun -p gpu -c 4 --gres=gpu:1 --pty bash -l
   (3) cd into Episcale_Signal
   (4) run build_EpiScale_Signal.sh to run cmake and put files in the 'build' folder using the command ./build_EpiScale_Signal.sh build
   
To run the code:
   (0) Stay logged into the gpu.
   (1) First load the modules used in build_Episcale_Signal.sh
   (2) Run the exacutable: ./bin/runDiscSimulation_M
To submit a job, use the SBATCH.sh file and run the command:
   (1) sbatch -p gpu --gres=gpu:1 --mem=5g --time=250:00:00 SBATCH.sh 
   This file runs the SBATCH.sh file with a request for 1 gpu with 5gb memory for 250 hours. 

To change clusters, you will need to change the modules in build_Episcale_Signal to the commands of the cluster in use. 
    For example, to use on the crc cluster, log into a gpu using the command qrsh -q gpu -l gpu_card=1
    Next, change the modules in the build_Episcale_Signal to: 
    module purge
    module load cmake
    module load cuda/9.1
    module load gcc/6.2.0
    module load matlab/2018b
    You will also need to change the following line since matlab has a slightly different location:
    -DMATLAB_DIR=$(dirname $(dirname $(dirname $(which matlab)))) \
