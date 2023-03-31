[BALE, HCLIB]
source ./oshmem-slurm.sh
cd hclib/modules/bale_actor/
rm -rf test
cp -r ../../../Conveyors-Design-Reinvented/test ./
source ./oshmem-slurm.sh
source ./oshmem-slurm.sh

[SCORE_P]
cd scorep-8.0/
mkdir _build && cd _build
../configure --prefix=/storage/home/hcoda1/8/ssinghal74/p-vsarkar9-1/ --with-otf2=/storage/home/hcoda1/8/ssinghal74/p-vsarkar9-1/ --with-cubew=/storage/home/hcoda1/8/ssinghal74/p-vsarkar9-1/ --with-cubelib=/storage/home/hcoda1/8/ssinghal74/p-vsarkar9-1/ 
make -j
make install
export PATH=$PATH:/storage/home/hcoda1/8/ssinghal74/p-vsarkar9-1/bin/

[CHECK]
module load qt/5.15.4-q74cbx
module load zlib/1.2.7-s3gke

[TMP full size: no space left on disk]
du -sk /tmp/* | sort -n

