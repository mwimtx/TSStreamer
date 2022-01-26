/***************************************************************************
**                                                                        **
**  TSStreamer Online Time Series Viewer                                  **
**  Copyright (C) 2019-2022 metronix GmbH                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
** Parts of the software are linked against the Open Source Version of Qt **
** Qt source code copy is available under https://www.qt.io               **
** Qt source code modifications are non-proprietary,no modifications made **
** https://www.qt.io/licensing/open-source-lgpl-obligations               **
****************************************************************************
**           Author: metronix geophysics                                  **
**  Website/Contact: www.metronix.de                                      **
**                                                                        **
**                                                                        **
****************************************************************************/

#include "fftreal_vector.h"

fftreal_vector::fftreal_vector()
{

}

fftreal_vector::~fftreal_vector()
{
 if (this->fft_object != nullptr) delete this->fft_object;

}

bool fftreal_vector::set_forward_buffer(std::vector<double> &inbuf, std::vector<std::complex<double>> &outbuf, const double freq, const bool bwincal, const bool isdcfft, const double cut_upper, const double cut_lower)
{
    if (!inbuf.size()) return false;
    this->wl = inbuf.size();

    this->psrc.resize(this->wl);
    if (this->fft_object != nullptr) delete this->fft_object;
    this->fft_object = new ffft::FFTReal<double>(this->wl);

    this->cut_upper = cut_upper;
    this->cut_lower = cut_lower;
    this->isdcfft = isdcfft;


    // we generate frequencies
    if (freq != 0) {
        this->fwl = (double) this->wl;
        this->f =  freq;
        if (bwincal) this->wincal = sqrt(1./double(f * (this->wl/2)) );
        this->start_idx = (size_t) ( (double)this->wl/2 * this->cut_lower);
        if (!this->start_idx && !this->isdcfft) start_idx = 1;
        this->stop_idx = (size_t) ( (double)this->wl/2 - ( (double)this->wl/2 * this->cut_upper)  );
        this->n = this->stop_idx - this->start_idx;
        outbuf.resize(n);
    }
    else {
        this->n = wl/2;
        outbuf.resize(n);
        this->stop_idx = this->wl/2;
    }


    return true;
}

void fftreal_vector::set_amplitude_spectra(std::vector<double> &fft_amplitude)
{
    fft_amplitude.reserve(n);
    fft_amplitude.resize(n);
    this->fft_amplitude = &fft_amplitude;
}

bool fftreal_vector::set_calibration(std::vector<std::complex<double>> &divcal)
{
    if (divcal.size() != this->wl/2) return false;
    this->divcal = &divcal;
    this->has_calibration = true;
    return true;
}

void fftreal_vector::unset_calibration()
{
    this->has_calibration = false;
}

void fftreal_vector::set_amplitude_spectra_stacked(std::vector<double> &fft_amplitude_stacked)
{
    fft_amplitude_stacked.reserve(n);
    fft_amplitude_stacked.resize(n);
    this->fft_amplitude_stacked = &fft_amplitude_stacked;
}

void fftreal_vector::fwd_process(std::vector<double> &inbuf, std::vector<std::complex<double> > &outbuf, const int ctrl)
{
    if (ctrl == fftreal_ctrl::hanning) {

        iter::hanning<double>(inbuf.begin(), inbuf.end());
    }
    else if (ctrl == fftreal_ctrl::detrend) {
        iter::detrend<double>(inbuf.begin(), inbuf.end());
    }
    else if (ctrl == fftreal_ctrl::detrend_hanning) {
        iter::detrend_and_hanning<double>(inbuf.begin(), inbuf.end());
    }

    // double sided FFT, left "output", right "input"; that is still the COMPLETE spectra
    fft_object->do_fft(&this->psrc[0], &inbuf[0]);

    //cout << "intern" << endl;
//    for (size_t zz = 0; zz < this->psrc.size(); ++zz) {
//        std::cout << this->psrc[zz] << " ";

//    }
//    std::cout << endl;

    // calibration should cover the complete frequency range!
    size_t j = 0;
    if (this->has_calibration && this->wincal != 0) {

        std::complex<double> tmp, result;

        for (size_t i = 0; i < wl/2; ++i) {
            tmp = std::complex<double> (psrc.at(i), -1. * psrc.at(wl/2 + i));
            result = tmp / this->divcal->operator [](i);
            //psrc[i] = result.real();
            //psrc[i] = result.imag();
            if (i >= this->start_idx && i < this->stop_idx) outbuf[j++] = result * this->wincal;
        }
    }
    if (!this->has_calibration && this->wincal != 0){
        j = 0;
        for (size_t i = this->start_idx; i < this->stop_idx; ++i) {

            outbuf[j++] = std::complex<double>(psrc.at(i), -1. * psrc.at(wl/2 + i)) * this->wincal;
        }
    }
    if (!this->has_calibration && this->wincal == 0){
        j = 0;
        for (size_t i = this->start_idx; i < this->stop_idx; ++i) {

            outbuf[j++] = std::complex<double>(psrc.at(i), -1. * psrc.at(wl/2 + i));
        }
    }


    if (this->fft_amplitude != nullptr) {
       for (j = 0; j < outbuf.size(); ++j) {
           this->fft_amplitude->operator [](j) = std::abs(outbuf.at(j));
       }
    }

    if (this->fft_amplitude_stacked != nullptr) {
        for (j = 0; j < outbuf.size(); ++j) {
            this->fft_amplitude_stacked->operator [](j) += std::abs(outbuf.at(j));
        }
    }

    if (this->inverse_from_forward) {
        fft_object->do_ifft(&this->psrc[0], &ts_calibrated->operator [](0));
        double length = this->ts_calibrated->size();
        for (size_t i = 0; i < this->ts_calibrated->size(); ++i) {
            this->ts_calibrated->operator [](i) /= length;
        }
    }



    ++this->counter;

}

bool fftreal_vector::set_inverse_buffer_after_forward(std::vector<double> &outbuf)
{
    outbuf.resize(this->wl);
    this->ts_calibrated = &outbuf;
    this->inverse_from_forward = true;

    return true;
}

bool fftreal_vector::unset_inverse_buffer_after_forward()
{
    this->inverse_from_forward = false;
    return true;
}


bool fftreal_vector::set_inverse_buffer(std::vector<std::complex<double>> &in_inv_buf, std::vector<double> &outbuf)
{
    if (!in_inv_buf.size()) return false;
    outbuf.resize(in_inv_buf.size() * 2);

    return true;
}


std::vector<double> fftreal_vector::get_frequencies()
{
    if (this->f) {

        this->freqs.resize(this->n);
        size_t j = 0, i = 0;
        for (i = 0; i < this->wl/2; ++i) {
            if (i >= this->start_idx && i < this->stop_idx) {
                this->freqs[j] = ( double(i) * (f/fwl) );
                ++j;
            }
        }
    }
    return freqs;
}

std::vector<double> fftreal_vector::get_frequencies_for_inverse() const
{
    std::vector<double> allfreqs(this->wl/2);
    if (this->f) {


        size_t i = 0;
        for (i = 0; i < this->wl/2; ++i) {
                allfreqs[i] = ( double(i) * (f/fwl) );
        }
    }
    return allfreqs;
}

size_t fftreal_vector::next_power_of_two(const size_t n) const
{
    size_t target, m;
    if (n > (SIZE_MAX - 1) / 2)
        throw "Vector too large";
    target = n;
    for (m = 1; m < target; m *= 2) {
        if (SIZE_MAX / 2 < m)
            throw "Vector too large";
    }

    return m;

}

void fftreal_vector::get_settings(double &freq, bool &bwincal, bool &isdcfft, double &cut_upper, double &cut_lower) const
{
    freq = this->f;
    if (wincal == 0) bwincal = false;
    else bwincal = true;
    isdcfft = this->isdcfft;
    cut_upper = this->cut_upper;
    cut_lower = this->cut_lower;
}


double fftreal_vector::get_nyquist_freq() const
{
    return ( double(this->wl/2) * (f/fwl) );
}

