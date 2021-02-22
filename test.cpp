#include "include/channel.hxx"

int main(int argc, char const *argv[])
{
  IDecorator<bool> input;
  //SymbolRepetitionCoder<bool, 3> rep_code{input};
  SymbolRepetitionDecoder<bool, 3> rep_decode{input};
  ODecorator<bool> output{rep_decode};


  bool data[]{0, 0, 1, 1, 1, 1};
  input.write(data, sizeof(data));

  std::unique_ptr<bool[]> out;
  for (int i = 0; i < sizeof(data)/3; i++)
  {
    if(i%sizeof(data)==0)
      out = std::move(output.request_block(sizeof(data)/3));
    std::cout << out[i%sizeof(data)] << " ";
  }
  std::cout << std::endl;
  return 0;
}
