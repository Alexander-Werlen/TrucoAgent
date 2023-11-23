#include <gtest/gtest.h>
#include "../../source/stateConverters/envidoConverter_NIC_to_NAEI.hpp"

EnvidoStateConverter converter;
TEST(envido_NIC_to_NAEI, envido_NIC_to_NAEI){
    string NIC, expected,res;
    //inicio ronda
    NIC= "0 00-00 [01e][02o][12c][01b][04c][04e] ";
    expected= "0 00-00 [01e][02o][12c] ";
    res=converter.convert_NIC_to_NAEI_envido(NIC);
    EXPECT_TRUE(expected == res)<<res;
    
    //juega P2
    NIC= "1 03-20 [01e][11o][12c][04b][04c][06e] (envido)";
    expected= "1 03-20 [04b][04c][06e] (envido)";
    res=converter.convert_NIC_to_NAEI_envido(NIC);
    EXPECT_TRUE(expected == res)<<res;

    //final1
    NIC= "0 23-00 [01e][11o][12c][04b][04c][06e] (envido)(real envido)(quiero)";
    expected= "2 23-00 [01e][11o][12c] (envido)(real envido)(quiero)";
    res=converter.convert_NIC_to_NAEI_envido(NIC);
    EXPECT_TRUE(expected == res)<<res;

    //final2
    NIC= "0 23-00 [01e][11o][12c][04b][04c][06e] (envido)(real envido)(falta envido)(quiero)";
    expected= "2 23-00 [01e][11o][12c] (envido)(real envido)(falta envido)(quiero)";
    res=converter.convert_NIC_to_NAEI_envido(NIC);
    EXPECT_TRUE(expected == res)<<res;

    //final3
    NIC= "0 23-00 [01e][11o][12c][04b][04c][06e] (11o)(04b)";
    expected= "2 23-00 [01e][11o][12c] (truco action)(truco action)";
    res=converter.convert_NIC_to_NAEI_envido(NIC);
    EXPECT_TRUE(expected == res)<<res;

    //conTrucoAction1
    NIC= "1 23-00 [01e][11o][12c][04b][04c][06e] (01e)";
    expected= "1 23-00 [04b][04c][06e] (truco action)";
    res=converter.convert_NIC_to_NAEI_envido(NIC);
    EXPECT_TRUE(expected == res)<<res;

    //long
    NIC= "1 23-00 [01e][11o][12c][04b][04c][06e] (11o)(envido)(falta envido)(no quiero)";
    expected= "2 23-00 [01e][11o][12c] (truco action)(envido)(falta envido)(no quiero)";
    res=converter.convert_NIC_to_NAEI_envido(NIC);
    EXPECT_TRUE(expected == res)<<res;
}
