//          Copyright Alain Miniussi 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Authors: Alain Miniussi

#ifndef BOOST_MPI_BROADCAST_HPP
#define BOOST_MPI_BROADCAST_HPP

namespace boost { namespace mpicxx {

class procid {
public:
  procid(int id) : m_id(id) {}
  int id() const { return m_id; }
private:
  int m_id;
};

template<class T>
class bcast_request {
  bcast_request(communicator const& comm, int src, int nb) 
    : m_comm(comm), m_source(src), m_buffer(0), m_nb(nb) {}
  bcast_request(bcast_request<T> const& req, T* buffer) 
    : m_comm(req.m_comm), m_source(req.m_src), m_buffer(buffer), m_nb(req.m_nb) {}

  int footprint() const { return sizeof(T)*m_nb; }
  bool load() {
    if (m_buffer == nullptr) {
      m_buffer = new T[m_nb];
    }
    m_err = MPI_Bcast(m_buffer, m_nb, mpi_datatype(*T), m_source, m_comm);
  }
  operator std::vector<T>() { return m_buffer

private:
  communicator const& m_comm;
  int                 m_source;
  T*                  m_buffer;
  int                 m_nb;
  int                 m_err;
};

template<typename T>
bcast_request<T> broadcast(communicator const& comm, procid src, T const* data, int n) {
  return bcast_request<T>(comm, src.id(), n);
}

template<typename T>
bcast_request<T> broadcast(communicator const& comm, procid src, int n) {
  return bcast_request<T>(comm, src.id(), n);
}

}}

#endif // BOOST_MPI_BROADCAST_HPP
