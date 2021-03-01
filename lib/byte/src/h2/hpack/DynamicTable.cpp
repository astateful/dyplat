#include "astateful/byte/h2/hpack/DynamicTable.hpp"
#include "astateful/byte/h2/hpack/HeaderField.hpp"

#include <cassert>
#include <iostream>

namespace astateful {
namespace byte {
namespace h2 {
namespace hpack {
  DynamicTable::DynamicTable( int capacity ) {
    setCapacity( capacity );
  }

  int DynamicTable::length() const {
    int length;
    if ( m_head < m_tail ) {
      length = m_data.size() - m_tail + m_head;
    } else {
      length = m_head - m_tail;
    }
    return length;
  }

  HeaderField& DynamicTable::get( int index ) const {
    assert( index > 0 && index <= length() );

    int i = m_head - index;
    if ( i < 0 ) return *m_data[ i + m_data.size() ];

    return *m_data[i];
  }

  void DynamicTable::add( const HeaderField& header ) {
    int header_size = header.size();
    if ( header_size > m_capacity ) {
      clear();
      return;
    }

    while ( m_size + header_size > m_capacity ) {
      remove();
    }

    m_data[m_head++] = std::make_unique<HeaderField>( header );
    m_size += header.size();
    if ( m_head == m_data.size() ) {
      m_head = 0;
    }
  }
 
  std::unique_ptr<HeaderField> DynamicTable::remove() {
    if ( m_data[m_tail] == nullptr ) return {};

    auto removed = std::move( m_data[m_tail] );

    m_size -= removed->size();
    //m_data[++m_tail] = nullptr;
    if ( ++m_tail == m_data.size() ) {
      m_tail = 0;
    }

    return std::move( removed );
  }

  void DynamicTable::clear() {
    while ( m_tail != m_head ) {
      m_data[m_tail++].release();
      if ( m_tail == m_data.size() ) {
        m_tail = 0;
      }
    }

    m_head = 0;
    m_tail = 0;
    m_size = 0;
  }

  void DynamicTable::setCapacity( int capacity ) {
    assert( capacity >= 0 );

    if ( m_capacity == capacity ) return;

    m_capacity = capacity;

    if ( capacity == 0 ) {
      clear();
    } else {
      // initially size will be 0 so remove won't be called
      while ( m_size > capacity ) {
        remove();
      }
    }

    int maxEntries = capacity / HEADER_ENTRY_OVERHEAD;
    if ( capacity % HEADER_ENTRY_OVERHEAD != 0 ) {
      maxEntries++;
    }

    // check if capacity change requires us to reallocate the array
    if ( m_data.size() == maxEntries ) return;

    std::vector<std::unique_ptr<HeaderField>> tmp( maxEntries );

    int len = length();
    int cursor = m_tail;

    for ( int i = 0; i < len; i++ ) {
      tmp[i] = std::move( m_data[cursor++] );
      if ( cursor == m_data.size() ) {
        cursor = 0;
      }
    }

    m_tail = 0;
    m_head = m_tail + len;
    m_data = std::move( tmp );
  }

  void DynamicTable::show() {
    auto len = length();

    /*for ( int i = 0; i < len; ++i ) {
      std::cout << m_data[i]->name;
      std::cout << " ";
      std::cout << m_data[i]->value;
      std::cout << std::endl;
    }*/
  }

  DynamicTable::~DynamicTable() {}
}
}
}
}