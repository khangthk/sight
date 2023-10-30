/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "resampler.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

#include <filter/image/resampler.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

resampler::resampler()
= default;

//------------------------------------------------------------------------------

resampler::~resampler()
= default;

//------------------------------------------------------------------------------

void resampler::configuring()
{
}

//------------------------------------------------------------------------------

void resampler::starting()
{
}

//------------------------------------------------------------------------------

void resampler::updating()
{
    const auto in_img    = m_image_in.lock();
    auto out_img         = m_image_out.lock();
    const auto target    = m_target_in.lock();
    const auto transform = m_transform_in.lock();

    SIGHT_ASSERT("No '" << IMAGE_IN << "' found !", in_img);
    SIGHT_ASSERT("No '" << IMAGE_IN << "' found !", out_img);
    SIGHT_ASSERT("No '" << TRANSFORM_IN << "' found !", transform);

    sight::filter::image::resampler::resample(
        in_img.get_shared(),
        out_img.get_shared(),
        transform.get_shared(),
        std::make_tuple(target->size(), target->origin(), target->spacing())
    );

    m_sig_computed->async_emit();

    auto img_buf_modified_sig = out_img->signal<data::image::buffer_modified_signal_t>
                                    (data::image::BUFFER_MODIFIED_SIG);

    img_buf_modified_sig->async_emit();

    auto img_modified_sig = out_img->signal<data::image::modified_signal_t>
                                (data::image::MODIFIED_SIG);

    img_modified_sig->async_emit();
}

//------------------------------------------------------------------------------

void resampler::stopping()
{
}

//------------------------------------------------------------------------------

service::connections_t resampler::auto_connections() const
{
    service::connections_t connections;
    connections.push(IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(TRANSFORM_IN, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(TARGET_IN, data::image::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

} // namespace sight::module::filter::image
