#include "../include/channel.hxx"

Decorator::~Decorator()
{
  if (m_next_on_heap)
  {
    delete m_next;
  }
}

Decorator::Array_t Decorator::evaluate(Data_t const *data, size_t block_size)
{
  auto data_pool = std::make_unique<Data_t[]>(block_size);
  std::memcpy((void *)data_pool.get(), (void const *)data, sizeof(Data_t) * block_size);
  return run(std::forward<SmartArr_t>(data_pool), block_size);
}
Decorator::Array_t Decorator::evaluate(std::basic_string_view<Data_t> data)
{
  auto data_pool = std::make_unique<Data_t[]>(data.size());
  std::memcpy((void *)data_pool.get(), (void const *)data.begin(), sizeof(Data_t) * data.size());
  return run(std::forward<SmartArr_t>(data_pool), data.size());
}
Decorator::Array_t Decorator::evaluate(std::vector<Data_t> const &data)
{
  auto data_pool = std::make_unique<Data_t[]>(data.size());
  std::memcpy((void *)data_pool.get(), (void const *)data.data(), sizeof(Data_t) * data.size());
  return run(std::forward<SmartArr_t>(data_pool), data.size());
}

Decorator &Decorator::operator>>(Decorator &next)
{
  return *(m_next = &next);
}

Decorator::RunReturn::RunReturn(Decorator *dec, Decorator::SmartArr_t &&fwd, size_t bs) : res{dec->runNext(std::forward<Decorator::SmartArr_t>(fwd), bs)}
{
}

Decorator::RunReturn RepetitionEncoder::run(SmartArr_t data, size_t block_size)
{
  auto block = std::make_unique<Decorator::Data_t[]>(block_size * n);
  for (size_t i = 0; i < block_size * n; i++)
    block[i] = data[i / n];
  return {this, std::move(block), block_size * n};
}

Decorator::RunReturn RepetitionDecoder::run(SmartArr_t data, size_t block_size)
{
  if (block_size % n)
    throw std::runtime_error("Incorrect block size");
  auto block = std::make_unique<Data_t[]>(block_size / n);
  uint_fast64_t sum = 0;
  for (size_t i = 0; i < block_size; i++)
  {
    if (!(i % n) && i)
    {
      block[i / n - 1] = sum > n / 2;
      sum = 0;
    }
    sum += !!data[i];
  }
  block[block_size / n - 1] = sum > n / 2;
  return {this, std::move(block), block_size / n};
}

Decorator::RunReturn ParityCheckEncoder::run(SmartArr_t data, size_t block_size)
{
  if (block_size % n)
    throw std::runtime_error("Incorrect block size");
  auto block = std::make_unique<Data_t[]>(block_size / n + block_size);
  bool parity = 0;
  for (size_t i = 0; i < block_size; i++)
  {
    if (!(i % n) && i)
    {
      block[i + i / n - 1] = parity;
      parity = 0;
    }
    parity ^= (block[i + i / n] = data[i]);
  }
  block[block_size / n + block_size - 1] = parity;
  return {this, std::move(block), block_size / n + block_size};
}

BinarySymmetricChannel::BinarySymmetricChannel(double bitflip_prob) : p{bitflip_prob}, m_bern_distr{p} {}

Decorator::RunReturn BinarySymmetricChannel::run(SmartArr_t data, size_t block_size)
{
  std::random_device rd{};
  std::mt19937 gen{rd()};
  for (size_t i = 0; i < block_size; i++)
    data[i] ^= m_bern_distr(gen);

  return {this, std::move(data), block_size};
}