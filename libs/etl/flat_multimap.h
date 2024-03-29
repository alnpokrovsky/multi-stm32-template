///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
http://www.etlcpp.com

Copyright(c) 2015 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef __ETL_FLAT_MULTMAP__
#define __ETL_FLAT_MULTMAP__

#include <stddef.h>
#include <iterator>
#include <algorithm>
#include <functional>
#include <utility>

#include "platform.h"
#include "vector.h"
#include "pool.h"
#include "exception.h"
#include "error_handler.h"
#include "debug_count.h"
#include "type_traits.h"
#include "parameter_type.h"

#undef ETL_FILE
#define ETL_FILE "3"

//*****************************************************************************
///\defgroup flat_multimap flat_multimap
/// A flat_multimapmap with the capacity defined at compile time.
/// Has insertion of O(N) and find of O(logN)
/// Duplicate entries and not allowed.
///\ingroup containers
//*****************************************************************************

namespace etl
{
  //***************************************************************************
  ///\ingroup flat_multimap
  /// Exception base for flat_multimaps
  //***************************************************************************
  class flat_multimap_exception : public etl::exception
  {
  public:

    flat_multimap_exception(string_type what, string_type file_name, numeric_type line_number)
      : exception(what, file_name, line_number)
    {
    }
  };

  //***************************************************************************
  ///\ingroup flat_multimap
  /// Vector full exception.
  //***************************************************************************
  class flat_multimap_full : public etl::flat_multimap_exception
  {
  public:

    flat_multimap_full(string_type file_name, numeric_type line_number)
      : etl::flat_multimap_exception(ETL_ERROR_TEXT("flat_multimap:full", ETL_FILE"A"), file_name, line_number)
    {
    }
  };

  //***************************************************************************
  ///\ingroup flat_multimap
  /// The base class for all templated flat_multimap types.
  //***************************************************************************
  class flat_multimap_base
  {
  public:

    typedef size_t size_type;

  protected:

    etl::debug_count construct_count;
  };

  //***************************************************************************
  /// The base class for specifically sized flat_multimaps.
  /// Can be used as a reference type for all flat_multimaps containing a specific type.
  ///\ingroup flat_multimap
  //***************************************************************************
  template <typename TKey, typename TMapped, typename TKeyCompare = std::less<TKey> >
  class iflat_multimap : public etl::flat_multimap_base
  {
  public:

    typedef std::pair<const TKey, TMapped> value_type;

  private:

    typedef etl::ivector<value_type*> lookup_t;
    typedef etl::ipool                storage_t;

  public:

    typedef TKey              key_type;
    typedef TMapped           mapped_type;
    typedef TKeyCompare       key_compare;
    typedef value_type&       reference;
    typedef const value_type& const_reference;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef size_t            size_type;

    //*************************************************************************
    class iterator : public std::iterator<std::bidirectional_iterator_tag, value_type>
    {
    public:

      friend class iflat_multimap;

      iterator()
      {
      }

      iterator(typename lookup_t::iterator ilookup)
        : ilookup(ilookup)
      {
      }

      iterator(const iterator& other)
        : ilookup(other.ilookup)
      {
      }

      iterator& operator =(const iterator& other)
      {
        ilookup = other.ilookup;
        return *this;
      }

      iterator& operator ++()
      {
        ++ilookup;
        return *this;
      }

      iterator operator ++(int)
      {
        iterator temp(*this);
        ++ilookup;
        return temp;
      }

      iterator& operator --()
      {
        --ilookup;
        return *this;
      }

      iterator operator --(int)
      {
        iterator temp(*this);
        --ilookup;
        return temp;
      }

      reference operator *()
      {
        return *(*ilookup);
      }

      const_reference operator *() const
      {
        return *(*ilookup);
      }

      pointer operator &()
      {
        return etl::addressof(*(*ilookup));
      }

      const_pointer operator &() const
      {
        return &(*(*ilookup));
      }

      pointer operator ->()
      {
        return etl::addressof(*(*ilookup));
      }

      const_pointer operator ->() const
      {
        return etl::addressof(*(*ilookup));
      }

      friend bool operator == (const iterator& lhs, const iterator& rhs)
      {
        return lhs.ilookup == rhs.ilookup;
      }

      friend bool operator != (const iterator& lhs, const iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      typename lookup_t::iterator ilookup;
    };

    //*************************************************************************
    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, const value_type>
    {
    public:

      friend class iflat_multimap;

      const_iterator()
      {
      }

      const_iterator(typename lookup_t::const_iterator ilookup)
        : ilookup(ilookup)
      {
      }

      const_iterator(const iterator& other)
        : ilookup(other.ilookup)
      {
      }

      const_iterator(const const_iterator& other)
        : ilookup(other.ilookup)
      {
      }

      const_iterator& operator =(const iterator& other)
      {
        ilookup = other.ilookup;
        return *this;
      }

      const_iterator& operator =(const const_iterator& other)
      {
        ilookup = other.ilookup;
        return *this;
      }

      const_iterator& operator ++()
      {
        ++ilookup;
        return *this;
      }

      const_iterator operator ++(int)
      {
        const_iterator temp(*this);
        ++ilookup;
        return temp;
      }

      const_iterator& operator --()
      {
        --ilookup;
        return *this;
      }

      const_iterator operator --(int)
      {
        const_iterator temp(*this);
        --ilookup;
        return temp;
      }

      const_reference operator *() const
      {
        return *(*ilookup);
      }

      const_pointer operator &() const
      {
        return etl::addressof(*(*ilookup));
      }

      const_pointer operator ->() const
      {
        return etl::addressof(*(*ilookup));
      }

      friend bool operator == (const const_iterator& lhs, const const_iterator& rhs)
      {
        return lhs.ilookup == rhs.ilookup;
      }

      friend bool operator != (const const_iterator& lhs, const const_iterator& rhs)
      {
        return !(lhs == rhs);
      }

    private:

      typename lookup_t::const_iterator ilookup;
    };

    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef typename std::iterator_traits<iterator>::difference_type difference_type;

  protected:

    typedef typename etl::parameter_type<TKey>::type key_parameter_t;

  private:

    //*********************************************************************
    /// How to compare elements and keys.
    //*********************************************************************
    class compare
    {
    public:

      bool operator ()(const value_type& element, key_type key) const
      {
        return key_compare()(element.first, key);
      }

      bool operator ()(key_type key, const value_type& element) const
      {
        return key_compare()(key, element.first);
      }
    };

  public:

    //*********************************************************************
    /// Returns an iterator to the beginning of the flat_multimap.
    ///\return An iterator to the beginning of the flat_multimap.
    //*********************************************************************
    iterator begin()
    {
      return iterator(lookup.begin());
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the flat_multimap.
    ///\return A const iterator to the beginning of the flat_multimap.
    //*********************************************************************
    const_iterator begin() const
    {
      return const_iterator(lookup.begin());
    }

    //*********************************************************************
    /// Returns an iterator to the end of the flat_multimap.
    ///\return An iterator to the end of the flat_multimap.
    //*********************************************************************
    iterator end()
    {
      return iterator(lookup.end());
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the flat_multimap.
    ///\return A const iterator to the end of the flat_multimap.
    //*********************************************************************
    const_iterator end() const
    {
      return const_iterator(lookup.end());
    }

    //*********************************************************************
    /// Returns a const_iterator to the beginning of the flat_multimap.
    ///\return A const iterator to the beginning of the flat_multimap.
    //*********************************************************************
    const_iterator cbegin() const
    {
      return const_iterator(lookup.cbegin());
    }

    //*********************************************************************
    /// Returns a const_iterator to the end of the flat_multimap.
    ///\return A const iterator to the end of the flat_multimap.
    //*********************************************************************
    const_iterator cend() const
    {
      return const_iterator(lookup.cend());
    }

    //*********************************************************************
    /// Returns an reverse iterator to the reverse beginning of the flat_multimap.
    ///\return Iterator to the reverse beginning of the flat_multimap.
    //*********************************************************************
    reverse_iterator rbegin()
    {
      return reverse_iterator(lookup.rbegin());
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the flat_multimap.
    ///\return Const iterator to the reverse beginning of the flat_multimap.
    //*********************************************************************
    const_reverse_iterator rbegin() const
    {
      return const_reverse_iterator(lookup.rbegin());
    }

    //*********************************************************************
    /// Returns a reverse iterator to the end + 1 of the flat_multimap.
    ///\return Reverse iterator to the end + 1 of the flat_multimap.
    //*********************************************************************
    reverse_iterator rend()
    {
      return reverse_iterator(lookup.rend());
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the end + 1 of the flat_multimap.
    ///\return Const reverse iterator to the end + 1 of the flat_multimap.
    //*********************************************************************
    const_reverse_iterator rend() const
    {
      return const_reverse_iterator(lookup.rend());
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the reverse beginning of the flat_multimap.
    ///\return Const reverse iterator to the reverse beginning of the flat_multimap.
    //*********************************************************************
    const_reverse_iterator crbegin() const
    {
      return const_reverse_iterator(lookup.crbegin());
    }

    //*********************************************************************
    /// Returns a const reverse iterator to the end + 1 of the flat_multimap.
    ///\return Const reverse iterator to the end + 1 of the flat_multimap.
    //*********************************************************************
    const_reverse_iterator crend() const
    {
      return const_reverse_iterator(lookup.crend());
    }

    //*********************************************************************
    /// Assigns values to the flat_multimap.
    /// If asserts or exceptions are enabled, emits flat_multimap_full if the flat_multimap does not have enough free space.
    /// If asserts or exceptions are enabled, emits flat_multimap_iterator if the iterators are reversed.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*********************************************************************
    template <typename TIterator>
    void assign(TIterator first, TIterator last)
    {
#if defined(ETL_DEBUG)
      difference_type count = std::distance(first, last);
      ETL_ASSERT(count <= difference_type(capacity()), ETL_ERROR(flat_multimap_full));
#endif

      clear();

      while (first != last)
      {
        insert(*first++);
      }
    }

    //*********************************************************************
    /// Inserts a value to the flat_multimap.
    /// If asserts or exceptions are enabled, emits flat_multimap_full if the flat_multimap is already full.
    ///\param value    The value to insert.
    //*********************************************************************
    std::pair<iterator, bool> insert(const value_type& value)
    {
      ETL_ASSERT(!lookup.full(), ETL_ERROR(flat_multimap_full));

      std::pair<iterator, bool> result(end(), false);

      iterator i_element = lower_bound(value.first);

      return insert_at(i_element, value);
    }

    //*********************************************************************
    /// Inserts a value to the flast_multi.
    /// If asserts or exceptions are enabled, emits flat_map_full if the flat_map is already full.
    ///\param position The position to insert at.
    ///\param value    The value to insert.
    //*********************************************************************
    iterator insert(iterator position, const value_type& value)
    {
      return insert(value).first;
    }

    //*********************************************************************
    /// Inserts a range of values to the flat_multimap.
    /// If asserts or exceptions are enabled, emits flat_multimap_full if the flat_multimap does not have enough free space.
    ///\param position The position to insert at.
    ///\param first    The first element to add.
    ///\param last     The last + 1 element to add.
    //*********************************************************************
    template <class TIterator>
    void insert(TIterator first, TIterator last)
    {
      while (first != last)
      {
        insert(*first++);
      }
    }

    //*********************************************************************
    /// Erases an element.
    ///\param key The key to erase.
    ///\return The number of elements erased. 0 or 1.
    //*********************************************************************
    size_t erase(key_parameter_t key)
    {
      std::pair<iterator, iterator> range = equal_range(key);

      if (range.first == end())
      {
        return 0;
      }
      else
      {
        size_t count = std::distance(range.first, range.second);
        erase(range.first, range.second);
        return count;
      }
    }

    //*********************************************************************
    /// Erases an element.
    ///\param i_element Iterator to the element.
    //*********************************************************************
    void erase(iterator i_element)
    {
      i_element->~value_type();
      storage.release(etl::addressof(*i_element));
      lookup.erase(i_element.ilookup);
      --construct_count;
    }

    //*********************************************************************
    /// Erases a range of elements.
    /// The range includes all the elements between first and last, including the
    /// element pointed by first, but not the one pointed by last.
    ///\param first Iterator to the first element.
    ///\param last  Iterator to the last element.
    //*********************************************************************
    void erase(iterator first, iterator last)
    {
      iterator itr = first;

      while (itr != last)
      {
        itr->~value_type();
        storage.release(etl::addressof(*itr));
        ++itr;
        --construct_count;
      }

      lookup.erase(first.ilookup, last.ilookup);
    }

    //*************************************************************************
    /// Clears the flat_multimap.
    //*************************************************************************
    void clear()
    {
      erase(begin(), end());
    }

    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator pointing to the element or end() if not found.
    //*********************************************************************
    iterator find(key_parameter_t key)
    {
      iterator itr = lower_bound(key);

      if (itr != end())
      {
        if (!key_compare()(itr->first, key) && !key_compare()(key, itr->first))
        {
          return itr;
        }
        else
        {
          return end();
        }
      }

      return end();
    }

    //*********************************************************************
    /// Finds an element.
    ///\param key The key to search for.
    ///\return An iterator pointing to the element or end() if not found.
    //*********************************************************************
    const_iterator find(key_parameter_t key) const
    {
      const_iterator itr = lower_bound(key);

      if (itr != end())
      {
        if (!key_compare()(itr->first, key) && !key_compare()(key, itr->first))
        {
          return itr;
        }
        else
        {
          return end();
        }
      }

      return end();
    }

    //*********************************************************************
    /// Counts an element.
    ///\param key The key to search for.
    ///\return 1 if the key exists, otherwise 0.
    //*********************************************************************
    size_t count(key_parameter_t key) const
    {
      std::pair<const_iterator, const_iterator> range = equal_range(key);

      return std::distance(range.first, range.second);
    }

    //*********************************************************************
    /// Finds the lower bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    iterator lower_bound(key_parameter_t key)
    {
      return std::lower_bound(begin(), end(), key, compare());
    }

    //*********************************************************************
    /// Finds the lower bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    const_iterator lower_bound(key_parameter_t key) const
    {
      return std::lower_bound(cbegin(), cend(), key, compare());
    }

    //*********************************************************************
    /// Finds the upper bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    iterator upper_bound(key_parameter_t key)
    {
      return std::upper_bound(begin(), end(), key, compare());
    }

    //*********************************************************************
    /// Finds the upper bound of a key
    ///\param key The key to search for.
    ///\return An iterator.
    //*********************************************************************
    const_iterator upper_bound(key_parameter_t key) const
    {
      return std::upper_bound(begin(), end(), key, compare());
    }

    //*********************************************************************
    /// Finds the range of equal elements of a key
    ///\param key The key to search for.
    ///\return An iterator pair.
    //*********************************************************************
    std::pair<iterator, iterator> equal_range(key_parameter_t key)
    {
      iterator i_lower = std::lower_bound(begin(), end(), key, compare());

      return std::make_pair(i_lower, std::upper_bound(i_lower, end(), key, compare()));
    }

    //*********************************************************************
    /// Finds the range of equal elements of a key
    ///\param key The key to search for.
    ///\return An iterator pair.
    //*********************************************************************
    std::pair<const_iterator, const_iterator> equal_range(key_parameter_t key) const
    {
      const_iterator i_lower = std::lower_bound(cbegin(), cend(), key, compare());

      return std::make_pair(i_lower, std::upper_bound(i_lower, cend(), key, compare()));
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    iflat_multimap& operator = (const iflat_multimap& rhs)
    {
      if (&rhs != this)
      {
        assign(rhs.cbegin(), rhs.cend());
      }

      return *this;
    }

    //*************************************************************************
    /// Gets the current size of the flat_multiset.
    ///\return The current size of the flat_multiset.
    //*************************************************************************
    size_type size() const
    {
      return lookup.size();
    }

    //*************************************************************************
    /// Checks the 'empty' state of the flat_multiset.
    ///\return <b>true</b> if empty.
    //*************************************************************************
    bool empty() const
    {
      return lookup.empty();
    }

    //*************************************************************************
    /// Checks the 'full' state of the flat_multiset.
    ///\return <b>true</b> if full.
    //*************************************************************************
    bool full() const
    {
      return lookup.full();
    }

    //*************************************************************************
    /// Returns the capacity of the flat_multiset.
    ///\return The capacity of the flat_multiset.
    //*************************************************************************
    size_type capacity() const
    {
      return lookup.capacity();
    }

    //*************************************************************************
    /// Returns the maximum possible size of the flat_multiset.
    ///\return The maximum size of the flat_multiset.
    //*************************************************************************
    size_type max_size() const
    {
      return lookup.max_size();
    }

    //*************************************************************************
    /// Returns the remaining capacity.
    ///\return The remaining capacity.
    //*************************************************************************
    size_t available() const
    {
      return lookup.available();
    }

  protected:

    //*********************************************************************
    /// Constructor.
    //*********************************************************************
    iflat_multimap(lookup_t& lookup_, storage_t& storage_)
      : lookup(lookup_),
      storage(storage_)
    {
    }

  private:

    //*********************************************************************
    /// Inserts a value to the flat_multimap.
    ///\param i_element The place to insert.
    ///\param value     The value to insert.
    //*********************************************************************
    std::pair<iterator, bool> insert_at(iterator i_element, const value_type& value)
    {
      std::pair<iterator, bool> result(end(), false);

      if (i_element == end())
      {
        // At the end.
        value_type* pvalue = storage.allocate<value_type>();
        ::new (pvalue) value_type(value);
        lookup.push_back(pvalue);
        result.first = --end();
        result.second = true;
      }
      else
      {
        // Not at the end.
        value_type* pvalue = storage.allocate<value_type>();
        ::new (pvalue) value_type(value);
        lookup.insert(i_element.ilookup, pvalue);
        result.first = i_element;
        result.second = true;
      }

      ++construct_count;

      return result;
    }

    // Disable copy construction.
    iflat_multimap(const iflat_multimap&);

    lookup_t&  lookup;
    storage_t& storage;
  };

  //***************************************************************************
  /// Equal operator.
  ///\param lhs Reference to the first flat_multimap.
  ///\param rhs Reference to the second flat_multimap.
  ///\return <b>true</b> if the arrays are equal, otherwise <b>false</b>
  ///\ingroup flat_multimap
  //***************************************************************************
  template <typename TKey, typename TMapped, typename TKeyCompare>
  bool operator ==(const etl::iflat_multimap<TKey, TMapped, TKeyCompare>& lhs, const etl::iflat_multimap<TKey, TMapped, TKeyCompare>& rhs)
  {
    return (lhs.size() == rhs.size()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  //***************************************************************************
  /// Not equal operator.
  ///\param lhs Reference to the first flat_multimap.
  ///\param rhs Reference to the second flat_multimap.
  ///\return <b>true</b> if the arrays are not equal, otherwise <b>false</b>
  ///\ingroup flat_multimap
  //***************************************************************************
  template <typename TKey, typename TMapped, typename TKeyCompare>
  bool operator !=(const etl::iflat_multimap<TKey, TMapped, TKeyCompare>& lhs, const etl::iflat_multimap<TKey, TMapped, TKeyCompare>& rhs)
  {
    return !(lhs == rhs);
  }

  //***************************************************************************
  /// A flat_multimap implementation that uses a fixed size buffer.
  ///\tparam TKey     The key type.
  ///\tparam TValue   The value type.
  ///\tparam TCompare The type to compare keys. Default = std::less<TKey>
  ///\tparam MAX_SIZE_ The maximum number of elements that can be stored.
  ///\ingroup flat_multimap
  //***************************************************************************
  template <typename TKey, typename TValue, const size_t MAX_SIZE_, typename TCompare = std::less<TKey> >
  class flat_multimap : public etl::iflat_multimap<TKey, TValue, TCompare>
  {
  public:

    static const size_t MAX_SIZE = MAX_SIZE_;

    //*************************************************************************
    /// Constructor.
    //*************************************************************************
    flat_multimap()
      : etl::iflat_multimap<TKey, TValue, TCompare>(lookup, storage)
    {
    }

    //*************************************************************************
    /// Copy constructor.
    //*************************************************************************
    flat_multimap(const flat_multimap& other)
      : etl::iflat_multimap<TKey, TValue, TCompare>(lookup, storage)
    {
      etl::iflat_multimap<TKey, TValue, TCompare>::assign(other.cbegin(), other.cend());
    }

    //*************************************************************************
    /// Constructor, from an iterator range.
    ///\tparam TIterator The iterator type.
    ///\param first The iterator to the first element.
    ///\param last  The iterator to the last element + 1.
    //*************************************************************************
    template <typename TIterator>
    flat_multimap(TIterator first, TIterator last)
      : etl::iflat_multimap<TKey, TValue, TCompare>(lookup, storage)
    {
      etl::iflat_multimap<TKey, TValue, TCompare>::assign(first, last);
    }

    //*************************************************************************
    /// Destructor.
    //*************************************************************************
    ~flat_multimap()
    {
      etl::iflat_multimap<TKey, TValue, TCompare>::clear();
    }

    //*************************************************************************
    /// Assignment operator.
    //*************************************************************************
    flat_multimap& operator = (const flat_multimap& rhs)
    {
      if (&rhs != this)
      {
        etl::iflat_multimap<TKey, TValue, TCompare>::assign(rhs.cbegin(), rhs.cend());
      }

      return *this;
    }

  private:

    typedef typename etl::iflat_multimap<TKey, TValue, TCompare>::value_type node_t;

    // The pool of nodes.
    etl::pool<node_t, MAX_SIZE> storage;

    // The vector that stores pointers to the nodes.
    etl::vector<node_t*, MAX_SIZE> lookup;
  };
}

#undef ETL_FILE

#endif
