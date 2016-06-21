//#include <iostream>
#include <cstdio>
#include <bitset>
#include <stdint.h>

int main(int argc, char* argv[]){
  //new one, two, three and four core ratios
  unsigned char newturbo[] = {27,25,25,25};
  //new TDP and TDC values, (value in W or A) * 8(granularity)
  uint16_t TDP = 65*8; //65W
  uint16_t TDC = 60*8; //60A
  //read and set turbo multipliers
  FILE *MSR = fopen("/dev/cpu/0/msr","rb");
  uint64_t cur_reg = 0;
  fseek(MSR,0x1ad,0);
  fread((void*)&cur_reg,8,1,MSR);
  unsigned char * c = (unsigned char*)&cur_reg;
  for (uint i = 0; i < 4; ++i){
    //std::cout << "Turbo C"<<i+1 << " " <<(uint)(c)[i] << " -> " << (uint)newturbo[i] << "\n";
    c[i] = newturbo[i];
  }
  fclose(MSR);
  MSR = fopen("/dev/cpu/0/msr","wb");
  fseek(MSR,0x1ad,0);
  fwrite(c,8,1,MSR);
  fclose(MSR);
  cur_reg = 0;
  //TDP/TDC settings
  MSR = fopen("/dev/cpu/0/msr","rb");
  fseek(MSR,0x1ac,0);
  fread((void*)&cur_reg,8,1,MSR);
  fclose(MSR);
  //create bitsets due to intels weird 14 bit numbers
  std::bitset<64> reg(cur_reg);
  std::bitset<16> TDPr(TDP);
  std::bitset<16> TDCr(TDC);
  for(uint i = 0; i < 14; ++i){
    //setting TDP
    reg[i] = TDPr[i];
    //setting TDC
    reg[i+16] = TDCr[i];
  }
  reg[15] = true; //enable TDP override
  reg[31] = true; //enable TDC override
  cur_reg = reg.to_ullong();
  //std::cout << cur_reg << "\n"
  MSR = fopen("/dev/cpu/0/msr","wb");
  fseek(MSR,0x1ac,0);
  fwrite(c,8,1,MSR);
  fclose(MSR);
  return 0;
}
