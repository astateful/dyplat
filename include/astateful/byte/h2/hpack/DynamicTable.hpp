#pragma once

#include <vector>
#include <memory>

namespace astateful {
namespace byte {
namespace h2 {
namespace hpack {
  //!
  struct HeaderField;

  //!
  struct DynamicTable final {
    DynamicTable( int );

    int length() const;

    int size() const { return m_size; }

    int capacity() const { return m_capacity; }

    HeaderField& get( int ) const;

    void add( const HeaderField& );

    std::unique_ptr<HeaderField> remove();

    void clear();

    void setCapacity( int );

    void show();

    ~DynamicTable();
  private:
    //!
    int m_capacity = -1;

    //!
    int m_size = 0;

    //!
    int m_head = 0;

    //!
    int m_tail = 0;

    //!
    std::vector<std::unique_ptr<HeaderField>> m_data;
  };
}
}
}
}