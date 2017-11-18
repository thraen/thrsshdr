#include "initalsa.h"

void alsa_setpar(snd_pcm_t *handle, char *device){
    unsigned int rate = 48000;
    unsigned int val, val2;
    int dir;
    snd_pcm_uframes_t   frames;
    snd_pcm_hw_params_t *hw_params;
    
    int err;

//  // open device
//  if ((err = snd_pcm_open( &handle, device, SND_PCM_STREAM_CAPTURE, 0 )) < 0) { 
//      fprintf(stderr, "cannot open audio device %s (%s)\n", device, snd_strerror (err)); exit (1); }

    // set parameters
    if ((val = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror (val)); exit (1);}
    if ((val = snd_pcm_hw_params_any (handle, hw_params)) < 0) {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (val)); exit (1); }
    if ((val = snd_pcm_hw_params_set_access (handle, hw_params, SND_PCM_ACCESS_RW_NONINTERLEAVED)) < 0) {
        fprintf(stderr, "cannot set access type (%s)\n", snd_strerror (val)); exit (1); }
    if ((val = snd_pcm_hw_params_set_format (handle, hw_params, SND_PCM_FORMAT_FLOAT64)) < 0) {
        fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror (val)); exit (1); }
    if ((val = snd_pcm_hw_params_set_rate_near (handle, hw_params, &rate, &dir)) < 0) {
        fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror (val)); exit (1); }
    if ((val = snd_pcm_hw_params_set_channels (handle, hw_params, 1)) < 0) {
        fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror (val)); exit (1); }
    if ((val = snd_pcm_hw_params (handle, hw_params)) < 0) {
        fprintf(stderr,"cannot set parameters (%s)\n", snd_strerror (val)); exit (1); }

    printf("\nPCM handle name = '%s'\n", snd_pcm_name(handle));
    printf(  "PCM state = %s\n", snd_pcm_state_name(snd_pcm_state(handle)));

    // wenn snd_pcm_hw_params fehlschlaegt, bleibt das device open. Dann allerdings sollte 
    // _hw_params <0 liefern, tut's aber nicht.
    // wenn das passiert versuch ich's einfach noch mal
    if (snd_pcm_state(handle) != SND_PCM_STATE_PREPARED)
        alsa_setpar(handle, device);

    snd_pcm_hw_params_get_access(hw_params, (snd_pcm_access_t *) &val);         printf("access type = %s\n", snd_pcm_access_name((snd_pcm_access_t)val));
    snd_pcm_hw_params_get_format(hw_params, (snd_pcm_format_t *) &val);         printf("format = '%s' (%s)\n", snd_pcm_format_name((snd_pcm_format_t)val), snd_pcm_format_description( (snd_pcm_format_t)val));
    snd_pcm_hw_params_get_subformat(hw_params, (snd_pcm_subformat_t *)&val);    printf("subformat = '%s' (%s)\n", snd_pcm_subformat_name((snd_pcm_subformat_t)val), snd_pcm_subformat_description((snd_pcm_subformat_t)val));
    snd_pcm_hw_params_get_channels(hw_params, &val);                            printf("channels = %d\n", val);
    snd_pcm_hw_params_get_rate(hw_params, &val, &dir);                          printf("rate = %d bps\n", val);
    snd_pcm_hw_params_get_period_time(hw_params, &val, &dir);                   printf("period time = %d us\n", val);
    snd_pcm_hw_params_get_period_size(hw_params, &frames, &dir);                printf("period size = %d frames\n", (int)frames);
    snd_pcm_hw_params_get_buffer_time(hw_params, &val, &dir);                   printf("buffer time = %d us\n", val);
    snd_pcm_hw_params_get_buffer_size(hw_params, (snd_pcm_uframes_t *) &val);   printf("buffer size = %d frames\n", val);
    snd_pcm_hw_params_get_periods(hw_params, &val, &dir);                       printf("periods per buffer = %d frames\n", val);
    snd_pcm_hw_params_get_rate_numden(hw_params, &val, &val2);                  printf("exact rate = %d/%d bps\n", val, val2);
    val = snd_pcm_hw_params_get_sbits(hw_params);                               printf("significant bits = %d\n", val);
//  snd_pcm_hw_params_get_tick_time(hw_params, &val, &dir);                     printf("tick time = %d us\n", val);
    val = snd_pcm_hw_params_is_batch(hw_params);                                printf("is batch = %d\n", val);
    val = snd_pcm_hw_params_is_block_transfer(hw_params);                       printf("is block transfer = %d\n", val);
    val = snd_pcm_hw_params_is_double(hw_params);                               printf("is double = %d\n", val);
    val = snd_pcm_hw_params_is_half_duplex(hw_params);                          printf("is half duplex = %d\n", val);
    val = snd_pcm_hw_params_is_joint_duplex(hw_params);                         printf("is joint duplex = %d\n", val);
    val = snd_pcm_hw_params_can_overrange(hw_params);                           printf("can overrange = %d\n", val);
    val = snd_pcm_hw_params_can_mmap_sample_resolution(hw_params);              printf("can mmap = %d\n", val);
    val = snd_pcm_hw_params_can_pause(hw_params);                               printf("can pause = %d\n", val);
    val = snd_pcm_hw_params_can_resume(hw_params);                              printf("can resume = %d\n", val);
    val = snd_pcm_hw_params_can_sync_start(hw_params);                          printf("can sync start = %d\n", val);

    snd_pcm_hw_params_free (hw_params);
}

