#pragma once
//
// INVARIANT: w_deleted_count <= w_deleting_count <= w_cached_read_count <= shared_read_count <= r_reading_count <= r_cached_write_count <= shared_write_count <= w_writing_count <= w_deleted_count + MAX_SIZE
//
// INVARIANT:      w_writing_count      - w_deleted_count     <= MAX_SIZE
// =========>      w_writing_count      - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - shared_read_count   <= MAX_SIZE
//
//
// INVARIANT: 0 <= r_cached_write_count - r_reading_count
// =========> 0 <= r_cached_write_count - shared_read_count
// =========> 0 <= shared_write_count   - shared_read_count
//
//
// THEOREM: =========> 0 <= shared_write_count   - shared_read_count   <= MAX_SIZE
//

#include <atomic>

//
//
// Channel/Queue:
//
// Shared between Producer and Consumer
//

struct channel_t{

  public:
    std::atomic<uint> read_addr, write_addr;
    //insert your code here

  public:

    //
    // Intialize
    //
    channel_t(){
      read_addr=0;
      write_addr=0;
      // insert your code here

    }
};


//
// Producer's (not shared)
//
struct writeport_t{
public:
    //insert your code here
  uint read_addr;
  uint write_addr;
  uint deleted_addr;
  uint deleting_addr;
  size_t buffer_length;

public:

  //
  // Intialize
  //
  writeport_t(size_t tsize)
  {
    //insert code here
    read_addr=0;
    write_addr=0;
    deleted_addr=0;
    deleting_addr=0;
    buffer_length=tsize;
  }

public:

  //
  // no of entries available to write
  //
  // helper function for write_canreserve
  //
  size_t write_reservesize(){

    // insert your code here
    return (size_t)(buffer_length-(write_addr-deleted_addr));
    //return avail_len;
  }

  //
  // Can write 'n' entries?
  //
  bool write_canreserve(size_t n){

    // insert your code here
    if(n<=write_reservesize()){
      return true;
    }
    return false;
  }

  //
  // Reserve 'n' entries for write
  //
  size_t write_reserve(size_t n){
    // insert your code here
    uint temp=write_addr;
    write_addr=write_addr+(uint)n;
    return (size_t)(temp%(uint)buffer_length);
  }

  //
  // Commit
  //
  // Read/Write shared memory data structure
  //
  void write_release(channel_t& ch){

    // insert your code here
    ch.write_addr=write_addr;

  }




public:

  //
  //
  // Read/Write shared memory data structure
  //
  void read_acquire(channel_t& ch){

    //insert your code here
    read_addr=ch.read_addr;

  }

  //
  // No of entires available to delete
  //
  size_t delete_reservesize(){
    //insert your code here
    return (size_t)(read_addr-deleted_addr);
  }

  //
  // Can delete 'n' entires?
  //
  bool delete_canreserve(size_t n){
    //insert your code here
    if(n<=delete_reservesize()){
      return true;
    }
    return false;
  }

  //
  // Reserve 'n' entires for deletion
  //
  size_t delete_reserve(size_t n){
    //insert your code here
    uint temp = deleting_addr;
    deleting_addr=deleting_addr+(uint)n;
    return (size_t)(temp%(uint)buffer_length);
  }


  //
  // Update the state, if any.
  //
  void delete_release(){
    //insert your code here
    deleted_addr=deleting_addr;
  }


};


//
// Consumer's (not shared)
//
//
struct readport_t{
public:

  //insert your code here
  uint read_addr,write_addr;
  size_t buffer_length;


public:
  //
  // Initialize
  //
  readport_t(size_t tsize)
  {
    read_addr=0;
    write_addr=0;
    buffer_length=tsize;
    //insert your code here

  }
  public:

  //
  // Read/Write shared memory data structure
  //
  void write_acquire(channel_t& ch){

    //insert your code here
    write_addr=ch.write_addr;

  }

  //
  // no of entries available to read
  //
  size_t read_reservesize(){

    //insert your code here
    return (size_t)(write_addr-read_addr);
   // return read_len;
  }

  //
  // Can Read 'n' entires?
  //
  bool read_canreserve(size_t n){

    //insert your code here
    if(n<=read_reservesize()){
      return true;
    }
    return false;
  }

  //
  // Reserve 'n' entires to be read
  //
  size_t read_reserve(size_t n){

    //insert your code here
    uint temp = read_addr;
    read_addr=read_addr+(uint)n;
    return (size_t)(temp%(uint)buffer_length);
  }

  //
  // Read/write shared memory data structure
  //
  void read_release(channel_t& ch){

    //insert your code here
    ch.read_addr=read_addr;
  }

};