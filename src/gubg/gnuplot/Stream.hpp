#ifndef HEADER_gubg_gnuplot_Stream_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gnuplot_Stream_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"
#include "gubg/mss.hpp"
#include <sstream>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>

namespace gubg { namespace gnuplot { 

    class Stream
    {
        public:
            using DataIX = int;

        private:
            using Line = std::string;
            using Lines = std::list<Line>;
            struct Info
            {
                unsigned int col_count = 0;
                unsigned int max_nr_cols = 0;
                Lines lines;
                std::string name;
            };
            using Part = std::map<DataIX, Info>;

        public:
            Stream()
            {
                add_new_part_();
            }

            class Data
            {
                public:
                    Data(Info &info): info_(info)
                {
                    info_.col_count = 0;
                }
                    Data(Data &&dying): oss_(std::move(dying.oss_)), info_(dying.info_) {}
                    ~Data()
                    {
                        info_.lines.push_back(oss_.str());
                        info_.max_nr_cols = std::max(info_.max_nr_cols, info_.col_count);
                    }

                    template <typename V>
                        Data &operator<<(const V &v)
                        {
                            oss_ << v << ' ';
                            ++info_.col_count;
                            return *this;
                        }
                private:
                    Data(const Data &);
                    Data &operator=(const Data &);

                    std::ostringstream oss_;
                    Info &info_;
            };

            Data data(DataIX ix = 0) { return Data(part_()[ix]); }
            Stream &name(DataIX ix, const std::string &n) { part_()[ix].name = n; return *this; }

            void pause() {add_new_part_();}

            void stream(std::ostream &os) const
            {
                for (const auto &data: parts_)
                {
                    stream_data_(data, os);
                    os << "pause mouse" << std::endl;
                }
            }
            std::string str() const
            {
                std::ostringstream oss;
                stream(oss);
                return oss.str();
            }
            void save(const std::string &fn) const
            {
                std::ofstream of(fn);
                stream(of);
            }

        private:
            static void stream_data_name_(std::ostream &os, DataIX ix) { os << "$data_" << ix; }
            static void stream_data_(const Part &part, std::ostream &os)
            {
                for (const auto &p: part)
                {
                    const auto ix = p.first;
                    const auto &lines = p.second.lines;
                    stream_data_name_(os, ix);
                    os << " << EOD" << std::endl;
                    for (const auto &line: lines)
                        os << line << std::endl;
                    os << "EOD" << std::endl;
                }

                OnlyOnce first_time;
                for (const auto &p: part)
                {
                    const auto ix = p.first;
                    const auto &info = p.second;
                    const auto nr_cols = info.max_nr_cols;
                    const auto &name = info.name;

                    auto add_plot = [&](unsigned int x, unsigned int y)
                    {
                        os << (first_time() ? "plot " : ", ");
                        stream_data_name_(os, ix);
                        os << " using " << x << ':' << y << " with lines ";
                        if (!name.empty())
                            os << "t \"" << name << "\" ";
                    };
                    if (nr_cols <= 0)
                    {
                    }
                    else if (nr_cols == 1)
                    {
                        //row ix vs. single column
                        add_plot(0, 1);
                    }
                    else
                    {
                        //first column vs. all the others
                        for (auto c = 2; c <= nr_cols; ++c)
                            add_plot(1, c);
                    }
                }
                os << std::endl;
            }
            using Parts = std::list<Part>;
            Parts parts_;
            Part &part_() { return parts_.back(); }
            const Part &part_() const { return parts_.back(); }
            void add_new_part_() {parts_.emplace_back();}
    };

} } 

#endif
