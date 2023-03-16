NO_ROW=10000
declare -a PAPI_eve=("PAPI_BR_CN" 
"PAPI_BR_INS" 
"PAPI_BR_MSP" 
"PAPI_BR_NTK" 
"PAPI_BR_PRC" 
"PAPI_BR_TKN" 
"PAPI_BR_UCN" 
"PAPI_CA_CLN"
"PAPI_CA_ITV"
"PAPI_CA_SHR"
"PAPI_CA_SNP"
"PAPI_DP_OPS"
"PAPI_FUL_CCY"
"PAPI_FUL_ICY"
"PAPI_L1_ICM"
"PAPI_L1_LDM"
"PAPI_L1_STM"
"PAPI_L1_TCM"
"PAPI_L2_DCA"
"PAPI_L2_DCM"
"PAPI_L2_DCR"
"PAPI_L2_DCW"
"PAPI_L2_ICA"
"PAPI_L2_ICH"
"PAPI_L2_ICM"
"PAPI_L2_ICR"
"PAPI_L2_LDM"
"PAPI_L2_STM"
"PAPI_L2_TCA"
"PAPI_L2_TCM"
"PAPI_L2_TCR"
"PAPI_L2_TCW"
"PAPI_L3_DCA"
"PAPI_L3_DCR"
"PAPI_L3_DCW"
"PAPI_L3_ICA"
"PAPI_L3_ICR"
"PAPI_L3_LDM"
"PAPI_L3_TCA"
"PAPI_L3_TCM"
"PAPI_L3_TCR"
"PAPI_L3_TCW"
"PAPI_LD_INS"
"PAPI_LST_INS"
"PAPI_MEM_WCY"
"PAPI_PRF_DM"
"PAPI_REF_CYC"
"PAPI_RES_STL"
"PAPI_SP_OPS"
"PAPI_SR_INS"
"PAPI_STL_CCY"
"PAPI_STL_ICY"
"PAPI_TLB_DM"
"PAPI_TLB_IM"
"PAPI_TOT_CYC"
"PAPI_TOT_INS"
"PAPI_VEC_DP"
"PAPI_VEC_SP")

mkdir pp
make triangle_conveyor
g++ exp2_compute_average.cpp -o exp2_res

# For every "PAPI event, on one model version, generate pp/ and run 

for str in ${PAPI_eve[@]}; do
    oshrun -n 24 ./triangle_conveyor -n $NO_ROW -P $str
    ./exp2_res -P $str
done

rm -rf pp