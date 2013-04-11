//NetTalk
/*------------------------------------------------------------------------------*\
 =============================
   Ä£¿éÃû³Æ: G729a.h
 =============================
 
 [°æÈ¨]
 
   VoiceAge Corporation                                                           *
 * 750, Chemin Lucerne                                                            *
 * Suite 250                                                                      *
 * Ville Mont-Royal (Quebec)                                                      *
 * Canada, H3R 2H6                                                                *
 *                                                                                *
 * Tel. (514) 737-4940, fax. (514) 908-2037 
                                              
\*------------------------------------------------------------------------------*/


/* va_g729a API functions prototypes and constants */

#define  L_FRAME_COMPRESSED 10
#define  L_FRAME            80

extern "C" void va_g729a_init_encoder();
extern "C" void va_g729a_encoder(short *speech, unsigned char *bitstream);
extern "C" void va_g729a_init_decoder();
extern "C" void va_g729a_decoder(unsigned char *bitstream, short *synth_short, int bfi);
