cd RDMA_PageRank
cmake ../../rdma/PageRank/
make
cd ..

cd RDMA_SSSP
cmake ../../rdma/SSSP/
make
cd ..

cd RDMA_WCC
cmake ../../rdma/WCC/
make
cd ..

cd Ethernet_IPoIB_PageRank
cmake ../../Ethernet_IPoIB/PageRank/
make
cd ..

cd Ethernet_IPoIB_SSSP
cmake ../../Ethernet_IPoIB/SSSP/
make
cd ..

cd Ethernet_IPoIB_WCC
cmake ../../Ethernet_IPoIB/WCC/
make
cd ..