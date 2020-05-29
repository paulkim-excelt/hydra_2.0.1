#
# Copyright: Copyright 2018 Broadcom Limited.
# This program is the proprietary software of Broadcom Limited
# and/or its licensors, and may only be used, duplicated, modified
# or distributed pursuant to the terms and conditions of a separate,
# written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized
# License, Broadcom grants no license (express or implied), right
# to use, or waiver of any kind with respect to the Software, and
# Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE
# NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
# IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
# ALL USE OF THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1.     This program, including its structure, sequence and organization,
# constitutes the valuable trade secrets of Broadcom, and you shall use
# all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of
# Broadcom integrated circuit products.
#
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
# PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
# REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
# OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
# DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
# NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
# ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
# CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
# OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
# BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
# INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
# ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
# TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
#

# Most of the following implementation is directly taken from the
# FixedPointNumbers.jl package. Modifications have been made to suit
# our needs.

import Base.+
import Base.-
import Base.*
import Base.<
import Base.show
import Base.typemin
import Base.typemax
import Base.convert

################################################################################
# Abstract fixed point type and helpers
################################################################################

# Umbrella FixedPoint type.
# T => Raw integer type
# f => Number of bits reserved for fractional part
abstract type BcmFixedPoint{T <: Integer, f} <: Real end

# This IOBuffer is used during module definition to generate typealias names
_iotypealias = IOBuffer()

# Printing. These are used to generate type-symbols, so we need them
# before we include any files.
function showtype(io::IO, ::Type{X}) where {X <: BcmFixedPoint}
    print(io, typestr(X))
    f = nbitsfrac(X)
    m = sizeof(X)*8-f-signbits(X)
    print(io, m, 'f', f)
    io
end

function show(io::IO, x::BcmFixedPoint{T,f}) where {T,f}
    show(io, round(convert(Float64,x), digits=ceil(Int,f/_log2_10)))
    get(io, :compact, false) || showtype(io, typeof(x))
end
const _log2_10 = 3.321928094887362

# Helpers
nbitsfrac(::Type{BcmFixedPoint{T,f}}) where {T <: Integer,f} = f
nbitsfrac(::Type{F}) where {F <: BcmFixedPoint} = nbitsfrac(supertype(F))

rawtype(::Type{BcmFixedPoint{T,f}}) where {T <: Integer,f} = T
rawtype(::Type{F}) where {F <: BcmFixedPoint} = rawtype(supertype(F))
rawtype(x::BcmFixedPoint) = rawtype(typeof(x))

# traits
typemax(::Type{T}) where {T <: BcmFixedPoint} = T(typemax(rawtype(T)), 0)
typemin(::Type{T}) where {T <: BcmFixedPoint} = T(typemin(rawtype(T)), 0)

# Common function to throw convert error
@noinline function throw_converterror(::Type{T}, x) where {T <: BcmFixedPoint}
    n = 2^(8*sizeof(T))
    bitstring = sizeof(T) == 1 ? "an 8-bit" : "a $(8*sizeof(T))-bit"
    io = IOBuffer()
    show(IOContext(io, :compact=>true), typemin(T)); Tmin = String(take!(io))
    show(IOContext(io, :compact=>true), typemax(T)); Tmax = String(take!(io))
    throw(ArgumentError("$T is $bitstring type representing $n values from $Tmin to $Tmax; cannot represent $x"))
end

# Return the raw representation
raw(x::T) where {T <: BcmFixedPoint} = x.i

################################################################################
# Fixed and Normed concrete types
################################################################################
struct BcmFixed{T <: Signed,f} <: BcmFixedPoint{T,  f}
    i::T

    BcmFixed{T, f}(i::Integer, _) where {T,f} = new{T, f}(i % T) # for setting by raw representation
end

struct BcmNormed{T<:Unsigned,f} <: BcmFixedPoint{T,f}
    i::T

    BcmNormed{T, f}(i::Integer,_) where {T,f} = new{T, f}(i%T)   # for setting by raw representation
end

typestr(::Type{X}) where {X <: BcmFixed} = "BcmQ"
signbits(::Type{X}) where {X <: BcmFixed} = 1

typestr(::Type{X}) where {X <: BcmNormed} = "BcmN"
signbits(::Type{X}) where {X <: BcmNormed} = 0

################################################################################
# Type aliases
################################################################################
for T in (Int8, Int16, Int32, Int64)
    for f in 0:sizeof(T)*8-1
        sym = Symbol(String(take!(showtype(_iotypealias, BcmFixed{T,f}))))
        @eval begin
            const $sym = BcmFixed{$T,$f}
            export $sym
        end
    end
end

for T in (UInt8, UInt16, UInt32, UInt64)
    for f in 0:sizeof(T)*8
        sym = Symbol(String(take!(showtype(_iotypealias, BcmNormed{T,f}))))
        @eval begin
            const $sym = BcmNormed{$T,$f}
            export $sym
        end
    end
end

################################################################################
# Conversions
################################################################################
# Convert float to Fixed
function BcmFixed{T, f}(x::AbstractFloat) where {T, f}
    rawf = trunc(x * (1 << f))

    if ((rawf < typemin(T)) || (rawf > typemax(T)))
        throw_converterror(Fixed{T, f}, x)
    end

    BcmFixed{T,f}(convert(T, rawf), 0)
end

function BcmNormed{T, f}(x::AbstractFloat) where {T, f}
    rawf = trunc(x * (1 << f))

    if (rawf > typemax(T))
        throw_converterror(Normed{T, f}, x)
    end

    BcmNormed{T, f}(convert(T, rawf), 0)
end


# Conversion between different fixed point representations.
function convert(::Type{TFP1}, x::TFP2) where {T1, f1, T2, f2, TFP1 <: BcmFixedPoint{T1, f1}, TFP2 <: BcmFixedPoint{T2, f2}}
    rawi = BigInt(x.i)

    shift = f1 - f2
    rawi = (shift >= 0) ? (rawi << shift) : (rawi >> abs(shift))

    if ((rawi < typemin(T1) || (rawi > typemax(T1))))
        throw_converterror(TFP1, x)
    end

    TFP1(rawi, 0)
end

# Convert BcmFixedPoint to float
function convert(::Type{T1}, x::BcmFixedPoint{T2, f}) where {T1 <: AbstractFloat, T2, f}
    return (x.i) / (1 << f)
end

################################################################################
# Addition and Multiplication
################################################################################
function +(x::TFP, y::TFP) where {T, f, TFP <: BcmFixedPoint{T, f}}
    rawi = BigInt(x.i) + BigInt(y.i)

    if ((rawi < typemin(T) || (rawi > typemax(T))))
        throw_converterror(T, x) # TODO
    end

    TFP(rawi, 0)
end

function -(x::TFP, y::TFP) where {T, f, TFP <: BcmFixedPoint{T, f}}
    rawi = BigInt(x.i) - BigInt(y.i)

    if ((rawi < typemin(T) || (rawi > typemax(T))))
        throw_converterror(T, x) # TODO
    end

    TFP(rawi, 0)
end

function *(x::Real, y::TFP) where {T, f, TFP <: BcmFixedPoint{T, f}}
    rawi = BigInt(trunc(Float64(x * (y.i))))

    if ((rawi < typemin(T) || (rawi > typemax(T))))
        throw_converterror(T, x) # TODO
    end

    TFP(rawi, 0)
end

################################################################################
# Comparisions
################################################################################
function <(x::TFP, y::TFP) where {T, f, TFP <: BcmFixedPoint{T, f}}
    (x.i) < (y.i)
end

