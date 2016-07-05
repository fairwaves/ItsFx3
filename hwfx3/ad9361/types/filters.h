#ifndef INCLUDED_TYPES_FILTERS_HPP
#define INCLUDED_TYPES_FILTERS_HPP

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include <sstream>


class filter_info_base
{
public:
    typedef std::shared_ptr<filter_info_base> sptr;
    enum filter_type
    {
        ANALOG_LOW_PASS,
        ANALOG_BAND_PASS,
        DIGITAL_I16,
        DIGITAL_FIR_I16
    };

    filter_info_base(
        filter_type type,
        bool bypass,
        size_t position_index
    ):
        _type(type), _bypass(bypass),
        _position_index(position_index)
    {
        //NOP
    }

    inline virtual bool is_bypassed()
    {
        return _bypass;
    }

    inline filter_type get_type()
    {
        return _type;
    }

    virtual ~filter_info_base()
    {
        //NOP
    }

    virtual std::string to_pp_string();

protected:
    filter_type _type;
    bool _bypass;
    size_t _position_index;

};

std::ostream& operator<<(std::ostream& os, filter_info_base& f);

class analog_filter_base : public filter_info_base
{
    std::string _analog_type;
    public:
        typedef std::shared_ptr<analog_filter_base> sptr;
        analog_filter_base(
            filter_type type,
            bool bypass,
            size_t position_index,
            const std::string& analog_type
        ):
            filter_info_base(type, bypass, position_index),
            _analog_type(analog_type)
        {
            //NOP
        }

        inline const std::string& get_analog_type()
        {
            return _analog_type;
        }

        virtual std::string to_pp_string();
};

class analog_filter_lp : public analog_filter_base
{
    double _cutoff;
    double _rolloff;

public:
    typedef std::shared_ptr<analog_filter_lp> sptr;
    analog_filter_lp(
        filter_type type,
        bool bypass,
        size_t position_index,
        const std::string& analog_type,
        double cutoff,
        double rolloff
    ):
        analog_filter_base(type, bypass, position_index, analog_type),
        _cutoff(cutoff),
        _rolloff(rolloff)
    {
        //NOP
    }

    inline double get_cutoff()
    {
        return _cutoff;
    }

    inline double get_rolloff()
    {
        return _cutoff;
    }

    inline void set_cutoff(const double cutoff)
    {
        _cutoff = cutoff;
    }

    virtual std::string to_pp_string();
};

template<typename tap_t>
class digital_filter_base : public filter_info_base
{
protected:
    double _rate;
    uint32_t _interpolation;
    uint32_t _decimation;
    tap_t _tap_full_scale;
    uint32_t _max_num_taps;
    std::vector<tap_t> _taps;

public:
    typedef std::shared_ptr<digital_filter_base> sptr;
    digital_filter_base(
        filter_type type,
        bool bypass,
        size_t position_index,
        double rate,
        size_t interpolation,
        size_t decimation,
        double tap_full_scale,
        size_t max_num_taps,
        const std::vector<tap_t>& taps
    ):
        filter_info_base(type, bypass, position_index),
        _rate(rate),
        _interpolation(interpolation),
        _decimation(decimation),
        _tap_full_scale(tap_full_scale),
        _max_num_taps(max_num_taps),
        _taps(taps)
    {
        //NOP
    }

    inline double get_output_rate()
    {
        return (_bypass ? _rate : (_rate / _decimation * _interpolation));
    }

    inline double get_input_rate()
    {
        return _rate;
    }

    inline double get_interpolation()
    {
        return _interpolation;
    }

    inline double get_decimation()
    {
        return _decimation;
    }

    inline double get_tap_full_scale()
    {
        return _tap_full_scale;
    }

    inline std::vector<tap_t>& get_taps()
    {
        return _taps;
    }

    virtual std::string to_pp_string()
    {
        std::ostringstream os;
        os<<filter_info_base::to_pp_string()<<
        "\t[digital_filter_base]"<<std::endl<<
        "\tinput rate: "<<_rate<<std::endl<<
        "\tinterpolation: "<<_interpolation<<std::endl<<
        "\tdecimation: "<<_decimation<<std::endl<<
        "\tfull-scale: "<<_tap_full_scale<<std::endl<<
        "\tmax num taps: "<<_max_num_taps<<std::endl<<
        "\ttaps: "<<std::endl;

        os<<"\t\t";
        for(size_t i = 0; i < _taps.size(); i++)
        {
            os<<"(tap "<<i<<": "<<_taps[i]<<")";
            if( ((i%10) == 0) && (i != 0))
            {
                os<<std::endl<<"\t\t";
            }
        }
        os<<std::endl;
        return std::string(os.str());
    }

};

template<typename tap_t>
class digital_filter_fir : public digital_filter_base<tap_t>
{
public:
    typedef std::shared_ptr<digital_filter_fir<tap_t> > sptr;

    digital_filter_fir(
        filter_info_base::filter_type type,
        bool bypass, size_t position_index,
        double rate,
        size_t interpolation,
        size_t decimation,
        size_t tap_bit_width,
        size_t max_num_taps,
        const std::vector<tap_t>& taps
    ):
        digital_filter_base<tap_t>(type, bypass, position_index, rate, interpolation, decimation, tap_bit_width, max_num_taps, taps)
    {
        //NOP
    }

    void set_taps(const std::vector<tap_t>& taps)
    {
        std::size_t num_taps = taps.size();
        if(num_taps < this->_max_num_taps){
            UHD_MSG(warning) << "digital_filter_fir::set_taps not enough coefficients. Appending zeros";
            std::vector<tap_t> coeffs;
            for (size_t i = 0; i < this->_max_num_taps; i++)
            {
                if(i < num_taps)
                {
                    coeffs.push_back(taps[i]);
                } else {
                    coeffs.push_back(0);
                }
            }
            this->_taps = coeffs;
        } else {
            this->_taps = taps;
        }
    }
};

#endif