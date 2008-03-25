#include "asf_meta.h"
#include "dateUtil.h"
#include "vexcel_plain.h"
#include "asf_nan.h"

meta_parameters* vp2meta(vexcel_plain *vp)
{
  /*
  meta_parameters *meta;
  char *mon[13]={"","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep",
     "Oct","Nov","Dec"};

  // Initialize the meta structure
  meta = raw_init();

  // Fill general block
  strcpy(meta->general->basename, vp->gli_product.image_desc.title);
  strcpy(meta->general->sensor, vp->sensor.title);
  strcpy(meta->general->sensor_name, MAGIC_UNSET_STRING); // Sensor name not available in ISP metadata
  strcpy(meta->general->mode, MAGIC_UNSET_STRING); // Mode not available in ISP metadata
  strcpy(meta->general->processor, MAGIC_UNSET_STRING); // Processor not available in ISP metadata
  if (strncmp(uc(vp->image_format), "FCOMPLEX", 8) == 0)
    meta->general->data_type = COMPLEX_REAL32;
  else if (strncmp(uc(vp->image_format), "SCOMPLEX", 8) == 0)
    meta->general->data_type = COMPLEX_INTEGER16;
  else if (strncmp(uc(vp->image_format), "FLOAT", 8) == 0) // Should never happen ...isp files should be SLCs
    meta->general->data_type = REAL32;
  else if (strncmp(uc(vp->image_format), "SHORT", 8) == 0) // Should never happen ...isp files should be SLCs
    meta->general->data_type = INTEGER16;
  else if (strncmp(uc(vp->image_format), "BYTE", 8) == 0) // Should never happen ...isp files should be SLCs
    meta->general->data_type = BYTE;
  switch(meta->general->data_type) {
    case COMPLEX_REAL32:
    case COMPLEX_INTEGER16:
      meta->general->image_data_type = COMPLEX_IMAGE;
      break;
    default:
      meta->general->image_data_type = IMAGE;
      break;
  }
  strcpy(meta->general->system, meta_get_system());
  sprintf(meta->general->acquisition_date, "%2d-%s-%4d",
    vp->acquisition.day, mon[vp->acquisition.month],
    vp->acquisition.year);
  meta->general->orbit = vp->orbit;
  if (vp->heading > 90.0 && vp->heading < 270.0)
    meta->general->orbit_direction = 'D';
  else
    meta->general->orbit_direction = 'A';
  meta->general->frame = MAGIC_UNSET_INT;
  meta->general->band_count = 1;
  strcpy(meta->general->bands, MAGIC_UNSET_STRING);
  meta->general->line_count = vp->azimuth_lines;
  meta->general->sample_count = vp->range_samples;
  meta->general->start_line = 0;
  meta->general->start_sample = 0;
  meta->general->x_pixel_size = vp->range_pixel_spacing;
  meta->general->y_pixel_size = vp->azimuth_pixel_spacing;
  meta->general->center_latitude = vp->center_latitude;
  meta->general->center_longitude = vp->center_longitude;
  meta->general->re_major = vp->earth_semi_major_axis;
  meta->general->re_minor = vp->earth_semi_minor_axis;
  meta->general->bit_error_rate = MAGIC_UNSET_DOUBLE;
  meta->general->missing_lines = 0;
  meta->general->no_data = MAGIC_UNSET_DOUBLE;

  // Fill SAR block
  meta->sar = meta_sar_init();
  if (strncmp(uc(vp->image_geometry), "SLANT_RANGE", 11) == 0)
    meta->sar->image_type = 'S';
  else if (strncmp(uc(vp->image_geometry), "GROUND_RANGE", 11) == 0)
    meta->sar->image_type = 'G';
  else
    meta->sar->image_type = MAGIC_UNSET_CHAR;
  if (vp->azimuth_angle >= 0.0)
    meta->sar->look_direction = 'R';
  else
    meta->sar->look_direction = 'L';
  meta->sar->look_count = vp->azimuth_looks;
  meta->sar->deskewed = vp->azimuth_deskew;
  meta->sar->original_line_count = meta->general->line_count;
  meta->sar->original_sample_count = meta->general->sample_count;
  meta->sar->line_increment = 1;
  meta->sar->sample_increment = 1;
  meta->sar->range_time_per_pixel = fabs((2.0 * vp->range_pixel_spacing) / SPD_LIGHT);
  meta->sar->azimuth_time_per_pixel = vp->azimuth_line_time;
  meta->sar->slant_range_first_pixel = vp->near_range_slc;
  meta->sar->slant_shift = 0.0;
  if (meta->general->orbit_direction == 'D')
    meta->sar->time_shift = 0.0;
  else if (meta->general->orbit_direction == 'A')
    meta->sar->time_shift = fabs(meta->sar->original_line_count * meta->sar->azimuth_time_per_pixel);
  else
    meta->sar->time_shift = MAGIC_UNSET_DOUBLE;
  meta->sar->wavelength = SPD_LIGHT / vp->radar_frequency;
  meta->sar->prf = vp->prf;
  meta->sar->earth_radius = vp->earth_radius_below_sensor;
  meta->sar->earth_radius_pp = meta->sar->earth_radius; // KLUDGE: This value is actually unknown in ISP metadata
  meta->sar->satellite_height = vp->sar_to_earth_center;
  strcpy(meta->sar->satellite_binary_time, MAGIC_UNSET_STRING);
  strcpy(meta->sar->satellite_clock_time, MAGIC_UNSET_STRING);
  int i;
  if (vp->doppler_polynomial[3] > 0.0001) {
    // FIXME: If this error ever fires, then we should insert a function that does a
    // quadratic fit to the cubic function.  Then we can derive close 'nuf quadratic
    // values from a set of points generated by the cubic and use those.
    asfPrintError("VP doppler polynomial has a large cubic term\n"
        "(%lf versus limit of 0.0001) and is not well modeled by a\nquadratic.",
        vp->doppler_polynomial[3]);
  }
  for (i=0; i<3; i++) {
    meta->sar->range_doppler_coefficients[i] = vp->doppler_polynomial[i];
    meta->sar->azimuth_doppler_coefficients[i] = 0.0; // FIXME: We have vp->radar_frequency and state vectors ...we should estimate the azimuth doppler stuff
  }
  meta->sar->azimuth_processing_bandwidth = vp->azimuth_proc_bandwidth;
  meta->sar->chirp_rate = vp->chirp_bandwidth;
  meta->sar->pulse_duration = MAGIC_UNSET_DOUBLE;
  meta->sar->range_sampling_rate = vp->adc_sampling_rate;
  strcpy(meta->sar->polarization, MAGIC_UNSET_STRING);
  meta->sar->multilook = 0;

  // Fill state vector structure
  meta->state_vectors = meta_state_vectors_init(3);
  meta->state_vectors = vp->stVec;

  return meta;
  */
}