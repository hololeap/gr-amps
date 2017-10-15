/* Written by Brandon Creighton <cstone@pobox.com>.
 *
 * This code is in the public domain; however, note that most of its
 * dependent code, including GNU Radio, is not.
 */

#ifndef INCLUDED_AMPS_FOCC_IMPL_H
#define INCLUDED_AMPS_FOCC_IMPL_H

//#define AMPS_DEBUG
//

#include <amps/focc.h>
#include <queue>
#include <itpp/comm/bch.h>
#include "amps_packet.h"
#include "amps_common.h"

using namespace itpp;
using std::string;
using boost::shared_ptr;

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif /* MAX */

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif /* MIN */

namespace gr {
  namespace amps {
      
    class focc_impl : public focc
    {
    private:
        // Queue of FOCC frames to emit when there's an empty (non-filler) slot
        std::queue<focc_frame *> frame_queue;
        boost::mutex frame_queue_mutex;

        std::queue<bool> d_bitqueue;    // Queue of symbols to be sent out.
        unsigned long d_symrate;        // output symbol rate (must be evenly divisible by the baud rate)
        bool d_aggressive_registration; // Enables aggressive registration superframe
        itpp::BCH bch;

        char *BI_zero_buf;        // Entire burst of symbols to send when B/I bit = 0
        char *BI_one_buf;         // Entire burst of symbols to send when B/I bit = 1

        const unsigned int samples_per_sym;
        burst_state cur_burst_state; 
#ifdef AMPS_DEBUG
        int debugfd;
#endif
        int cur_off;
        int cur_seg_idx;
        focc_frame *cur_frame;
        int cur_frame_idx;
        int cur_seg_len;
        char *cur_seg_data;

        std::vector<focc_frame *> superframe_frames;

        inline void queuebit(bool bit);
        inline unsigned long queuesize() { return d_bitqueue.size(); }
        void queue_dup(bvec &bv);
        void make_superframe();
        void make_registration_superframe();
        void validate_superframe();
        std::vector<char> focc_bch(std::vector<char> inbits);
        focc_frame *make_frame(std::vector<char> word_a, std::vector<char> word_b, bool ephemeral=false, bool filler=false);
        void next_burst_state();

    public:
        focc_impl(unsigned long symrate, bool aggressive_registration);
        ~focc_impl();

        void focc_words_message(pmt::pmt_t msg);
        void push_frame_queue(focc_frame *frame);
        focc_frame *pop_frame_queue();
        void queue_file();
        void queue(shared_ptr<bvec> bvptr);
        void queue(uint32_t val);
        int work(int noutput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };


  } // namespace amps
} // namespace gr

#endif /* INCLUDED_AMPS_FOCC_IMPL_H */

