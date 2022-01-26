#ifndef FFTREAL_VECTOR_H
#define FFTREAL_VECTOR_H

#include <vector>
#include <complex>
#ifdef _msvc
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include "iterator_templates.h"

#include "ffft/FFTReal.h"

#include <iostream>

enum fftreal_ctrl {
    nothing = 0,
    scale = 1,
    detrend = 2,
    hanning = 3,
    detrend_hanning = 4,
    scale_detrend_hanning = 5,
    calibrate = 6,
    inverse = 7,
    inverse_after_cabibration = 8,
    is_dc_ftt = 9
};


/*!
   \brief The fftreal_vector class is an adoption of the FFREAL for a vector
 */
class fftreal_vector
{

public:

    fftreal_vector();

    ~fftreal_vector();
    /*!
     * \brief set_forward_buffer is the main INITIALISATION; it also RESETS the pointers of ampl and cal vectors!
     * \param inbuf
     * \param outbuf complex number vector; will be resized
     * \param cut_upper
     * \param cut_lower
     * \param freq   when f is set the DC part is skipped automatically
     * \param window_cal respect the length of the FFT - so 1024 and 4096 should give the same result
     * \param keep the DC component in output - if yo create a frequency vector and use log display this will result into error log(0) -> error!
     * \param cut_upper cut upper frequencies from output
     * \param cut_lower cut lower frequencies from output
     */
    bool set_forward_buffer(std::vector<double> &inbuf, std::vector<std::complex<double>> &outbuf, const double freq = 0, const bool bwincal = true, const bool isdcfft = false,
                            const double cut_upper = 0, const double cut_lower = 0);


    void set_amplitude_spectra(std::vector<double> &fft_amplitude);

    bool set_calibration(std::vector<std::complex<double>> &divcal);

    /*!
       \brief unset_calibration - turns off te calibration flag
     */
    void unset_calibration();

    void set_amplitude_spectra_stacked(std::vector<double> &fft_amplitude_stacked);

    /*!
       \brief fwd_process does the fft.
       \param inbuf
       \param outbuf
       \param ctrl
     */
    void fwd_process(std::vector<double> &inbuf, std::vector<std::complex<double>> &outbuf, const int ctrl = fftreal_ctrl::hanning);

    /*!
     * \brief set_inverse_buffer_after_forward will calculate the inverse immedeately while calling fwd_process; settings for calibration have to be set in advance.
     *  hence that a inverse need a RECTANGULAR window!
     * \param outbuf
     * \return
     */
    bool set_inverse_buffer_after_forward(std::vector<double> &outbuf);

    bool unset_inverse_buffer_after_forward();

    /*!
       \brief set_inverse_buffer makes a inverse FFT after having done a calibration - in order to get calibrated time series
       \param inbuf
       \param outbuf
       \return
     */
    bool set_inverse_buffer(std::vector<std::complex<double>> &in_inv_buf, std::vector<double> &outbuf);



    /*!
       \brief get_frequencies
       \return list of frequencies for each point. Hence: using full FFT means that f[0] = 0 = DC part
     */
    std::vector<double> get_frequencies();

    /*!
       \brief get_frequencies_for_inverse
       \return list of frequencies for each point, including ALL frequencies, and f[0] = 0 = DC part
     */
    std::vector<double> get_frequencies_for_inverse() const;

    /*!
       \brief next_power_of_two
       \param n
       \return 16 if n = 15 or 32 if in = 17 and so on
     */
    size_t next_power_of_two(const size_t n) const;

    void get_settings(double &freq, bool &bwincal, bool &isdcfft, double &cut_upper, double &cut_lower) const;


    double get_nyquist_freq() const;



private:

    ffft::FFTReal <double> *fft_object = nullptr;           //!< create a fftreal

    size_t wl;                                              //!< input window length

    std::vector<double> psrc;                               //!< output is a half sided: pseudo real - complex; either make complex out of this or amplitudes
    std::vector<double> *fft_amplitude_stacked = nullptr;   //!< calculate the stacked amplitude spectra ( double )
    std::vector<double> *fft_amplitude         = nullptr;   //!< calculate the amplitude spectra ( double )
    std::vector<std::complex<double>> *divcal   = nullptr;  //!< calibrate spectra spectra ( double )
    bool has_calibration = false;                           //!< want to calibrate
    bool inverse_from_forward = false;                      //!< if you want calibrated time series in nT we immedeately calculate the inverse
    std::vector<double> *ts_calibrated        = nullptr;    //!< calculate the inverse FFT for calibrated time series ( double )

    //    bool isscale = false;
    bool isdcfft = false;
    //    bool is_detrend_hanning = 0;
    double wincal = 0;
    double fwl = 0;                                         //!< double window length
    double f = 0;                                           //!< set the sampling frequency - a frequency vector can be generated
    size_t n = 0;                                           //!< size of the complex spectra output
    std::vector<double> freqs;                              //!< a frequency vector can be generated


    double cut_upper = 0;                                   //!<  for MT the complete spectra may not be needed; use 0.05 to cut some upper
    double cut_lower = 0;                                   //!<  for MT the complete spectra may not be needed; use 0.1 to cut 10% lower

    size_t start_idx;                                       //!< start index in case we use not the full spectra
    size_t stop_idx;                                        //!< stop index in case we use not the full spectra

    size_t counter = 0;                                     //!< count the ffts (needed for thread control)

};

#endif // FFTREAL_VECTOR_H
