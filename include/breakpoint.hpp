#ifndef MINIDBG_BREAKPOINT_HPP
#define MINIDBG_BREAKPOINT_HPP

namespace dbg {
	struct prg_data;
    class breakpoint {
    public:
        breakpoint();
		breakpoint(std::shared_ptr<prg_data> , std::intptr_t );
		~breakpoint();
		breakpoint(const breakpoint&);
		breakpoint& operator=(const breakpoint&);
		breakpoint(breakpoint&& );
		breakpoint& operator=(breakpoint&&);
		void set_brk();
		void unset_brk();

    private:
		std::shared_ptr<prg_data> p_prg_data;
		struct brkpt_data;
		std::unique_ptr<brkpt_data> p_brkpt_data;
    };
}

#endif
