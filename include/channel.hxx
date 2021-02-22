#ifndef CHANNEL_HXX
#define CHANNEL_HXX

#include <random>
#include <iostream>
#include <string>
#include <concepts>
#include <functional>
#include <memory>
#include <sstream>
#include <cstring>

template <std::integral Data_t>
class Decorator
{
public:
  Decorator() = default;
  Decorator(Decorator &parent) : m_parent_decor{&parent} {};
  virtual ~Decorator() = default;
  virtual std::unique_ptr<Data_t[]> request_block(size_t block_size) = 0;

protected:
  std::unique_ptr<Data_t[]> request_from_parent(size_t block_size)
  {
    return std::forward<std::unique_ptr<Data_t[]>>(m_parent_decor->request_block(block_size));
  }

private:
  Decorator *m_parent_decor = 0;
};

template <std::integral Data_t>
class IDecorator : public Decorator<Data_t>
{
public:
  IDecorator() = default;
  std::unique_ptr<Data_t[]> request_block(size_t block_size) override
  {
    auto block = std::make_unique<Data_t[]>(block_size);
    m_input.read(block.get(), block_size);
    m_input.clear();
    return std::move(block);
  }
  void write(std::basic_string_view<Data_t> src)
  {
    m_input.write(src.begin(), src.size());
  }
  void write(std::basic_string<Data_t> const &src)
  {
    m_input.write(src.c_str(), src.size());
  }
  void write(Data_t const *src, std::size_t size)
  {
    m_input.write(src, size);
  }

private:
  std::basic_stringstream<Data_t> m_input;
};

template <std::integral Data_t>
class ODecorator : public Decorator<Data_t>
{
public:
  ODecorator(Decorator<Data_t> &parent) : Decorator<Data_t>{parent} {};
  std::unique_ptr<Data_t[]> request_block(size_t block_size)
  {
    return std::forward<std::unique_ptr<Data_t[]>>(Decorator<Data_t>::request_from_parent(block_size));
  }
  void read(Data_t *buffer, std::size_t buf_size)
  {
    auto block = Decorator<Data_t>::request_from_parent(buf_size);
    std::memcpy(buffer, block.get(), buf_size);
  }

private:
};

template <std::integral Data_t, size_t N>
class SymbolRepetitionCoder : public Decorator<Data_t>
{
public:
  static constexpr size_t repetition_number = N;
  SymbolRepetitionCoder(Decorator<Data_t> &parent) : Decorator<Data_t>{parent} {};
  std::unique_ptr<Data_t[]> request_block(size_t block_size)
  {
    auto block = std::make_unique<Data_t[]>(block_size);
    auto required_data_size = (block_size - m_prev_count);
    required_data_size = required_data_size / repetition_number + static_cast<bool>(required_data_size % repetition_number);
    auto recv_block = Decorator<Data_t>::request_from_parent(required_data_size);
    size_t i = 0;

    size_t size = 0;
    while (size < block_size)
    {
      if (!m_prev_count)
      {
        m_prev_count = N;
        m_previous = recv_block[i++];
      }
      block[size++] = m_previous;
      --m_prev_count;
    }
    return std::move(block);
  }

private:
  Data_t m_previous;
  size_t m_prev_count = 0;
};

template <std::integral Data_t, size_t N>
class SymbolRepetitionDecoder : public Decorator<Data_t>
{
public:
  static constexpr size_t repetition_number = N;
  SymbolRepetitionDecoder(Decorator<Data_t> &parent) : Decorator<Data_t>{parent} {};
  std::unique_ptr<Data_t[]> request_block(size_t block_size)
  {
    auto block = std::make_unique<Data_t[]>(block_size);
    auto recv_block = Decorator<Data_t>::request_from_parent(block_size * repetition_number);
    size_t size = 0;
    for (size_t i = 0; i < block_size; i++)
    {
      Data_t symbs[N];
      size_t count[N]{0};

      for (size_t j = 0; j < repetition_number; j++)
      {
        size_t k = 0;
        for (; count[k]; k++)
          if (symbs[k] == recv_block[i * repetition_number + j])
            break;
        symbs[k] = recv_block[i * repetition_number + j];
        ++count[k];
      }
      size_t maxpos = 0;
      for (size_t j = 0; count[j]; j++)
        if (count[j] > count[maxpos])
          maxpos = j;
      block[size++] = symbs[maxpos];
    }
    return std::move(block);
  }

private:
  Data_t m_previous;
  size_t m_prev_count = 0;
};


template <size_t N>
class ParityCoder : public Decorator<bool>
{
public:
  static constexpr size_t section_length = N;
  ParityCoder(Decorator<bool> &parent) : Decorator<bool>{parent} {};
  std::unique_ptr<bool[]> request_block(size_t block_size)
  {
    auto block = std::make_unique<bool[]>(block_size);
    auto required_data_size = (block_size - m_count + m_first);
    //required_data_size = required_data_size / section_length + static_cast<bool>(required_data_size % section_length);
    // auto required_data_size = (block_size - m_prev_count);
    // required_data_size = required_data_size / repetition_number + static_cast<bool>(required_data_size % repetition_number);
    // auto recv_block = Decorator<Data_t>::request_from_parent(required_data_size);
    // size_t i = 0;

    // size_t size = 0;
    // while (size < block_size)
    // {
    //   if (!m_prev_count)
    //   {
    //     m_prev_count = N;
    //     m_previous = recv_block[i++];
    //   }
    //   block[size++] = m_previous;
    //   --m_prev_count;
    // }
    return std::move(block);
  }

private:
  bool m_previous[N];
  size_t m_first = 0; 
  size_t m_count = 0;
};
// template <std::integral Data_t>
// class DataOStream {
//   public:
//   virtual ~DataOStream() = default;
//   void write(Data_t *buffer, size_t data_size);
//   private:
// };

// template <std::integral Data_t>
// class DataIStream
// {
// public:
//   virtual ~DataIStream() = default;
//   void read(Data_t *buffer, size_t buffer_size);
//   std::size_t size() const;

// private:
//   std::function<std::size_t() const> m_size_callback;
//   std::function<std::unique_ptr<Data_t[]>(std::size_t)> m_source;
// };

// template <std::integral Data_t>
// class Coder
// {
// public:
//   virtual DataIStream<Data_t> encode(std::basic_stringstream<Data_t> &input) = 0;
//   virtual DataIStream<Data_t> decode(std::basic_stringstream<Data_t> &input) = 0;
//   virtual ~Coder() = default;

// private:
// };

// class Channel
// {
// public:
//   virtual std::basic_string<int32_t> pass_through(std::basic_string<int32_t> &const) = 0;
//   virtual ~Channel() = default;

// private:
// };

// class BinarySymmetricChannel : public Channel
// {

// public:
// private:
// };

#endif // CHANNEL_HXX
