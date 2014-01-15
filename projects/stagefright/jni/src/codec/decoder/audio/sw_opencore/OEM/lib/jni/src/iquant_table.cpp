/* ------------------------------------------------------------------
 * Copyright (C) 2008 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */
/*

 Pathname: ./src/iquant_table.c

     Date: 05/10/2001

------------------------------------------------------------------------------
 REVISION HISTORY

 Description:

------------------------------------------------------------------------------
 INPUT AND OUTPUT DEFINITIONS

 None, just contains tables.
------------------------------------------------------------------------------
 FUNCTION DESCRIPTION

 Holds a table used for esc_iquant, containing the values of x^1/3 in
 Q format.
------------------------------------------------------------------------------
 REQUIREMENTS

------------------------------------------------------------------------------
 REFERENCES

 (1) ISO/IEC 13818-7:1997 Titled "Information technology - Generic coding
   of moving pictures and associated audio information - Part 7: Advanced
   Audio Coding (AAC)", Section 10.3, "Decoding process", page 43.


------------------------------------------------------------------------------
 PSEUDO-CODE
 None.

------------------------------------------------------------------------------
 RESOURCES USED
 None.
------------------------------------------------------------------------------
*/


/*----------------------------------------------------------------------------
; INCLUDES
----------------------------------------------------------------------------*/
#include "pv_audio_type_defs.h"
#include "iquant_table.h"

/*----------------------------------------------------------------------------
; MACROS
; Define module specific macros here
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; DEFINES
; Include all pre-processor statements here. Include conditional
; compile variables also.
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; LOCAL FUNCTION DEFINITIONS
; Function Prototype declaration
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; LOCAL STORE/BUFFER/POINTER DEFINITIONS
; Variable declaration - defined here and used outside this module
----------------------------------------------------------------------------*/

/*
   This table contains the value of x ^ (1/3) where x is in the range of
   [0..1024], in Q27 format.
   Note that the length of the table is 1025, and not 1024 - this is because
   the function esc_iquant may need to do an interpolation for numbers near
   8191, which in that case it needs to get 8192 ^(1/3).
 */
const UInt32 inverseQuantTable[] =
{
    0x00000000, /*    0 */
    0x08000000, /*    1 */
    0x0a14517d, /*    2 */
    0x0b89ba25, /*    3 */
    0x0cb2ff53, /*    4 */
    0x0dae07de, /*    5 */
    0x0e897685, /*    6 */
    0x0f4daedd, /*    7 */
    0x10000000, /*    8 */
    0x10a402fd, /*    9 */
    0x113c4841, /*   10 */
    0x11cab613, /*   11 */
    0x1250bfe2, /*   12 */
    0x12cf8890, /*   13 */
    0x1347f8ab, /*   14 */
    0x13bacd65, /*   15 */
    0x1428a2fa, /*   16 */
    0x1491fc15, /*   17 */
    0x14f74744, /*   18 */
    0x1558e2f7, /*   19 */
    0x15b72095, /*   20 */
    0x161246d7, /*   21 */
    0x166a9399, /*   22 */
    0x16c03d55, /*   23 */
    0x17137449, /*   24 */
    0x17646369, /*   25 */
    0x17b33124, /*   26 */
    0x18000000, /*   27 */
    0x184aef29, /*   28 */
    0x18941ad8, /*   29 */
    0x18db9cb7, /*   30 */
    0x19218c2e, /*   31 */
    0x1965fea5, /*   32 */
    0x19a907c2, /*   33 */
    0x19eab998, /*   34 */
    0x1a2b24d0, /*   35 */
    0x1a6a58d5, /*   36 */
    0x1aa863ee, /*   37 */
    0x1ae5535d, /*   38 */
    0x1b213377, /*   39 */
    0x1b5c0fbd, /*   40 */
    0x1b95f2ec, /*   41 */
    0x1bcee70f, /*   42 */
    0x1c06f590, /*   43 */
    0x1c3e2745, /*   44 */
    0x1c74847a, /*   45 */
    0x1caa1501, /*   46 */
    0x1cdee035, /*   47 */
    0x1d12ed0b, /*   48 */
    0x1d464212, /*   49 */
    0x1d78e582, /*   50 */
    0x1daadd3a, /*   51 */
    0x1ddc2ecf, /*   52 */
    0x1e0cdf8c, /*   53 */
    0x1e3cf476, /*   54 */
    0x1e6c7257, /*   55 */
    0x1e9b5dba, /*   56 */
    0x1ec9baf6, /*   57 */
    0x1ef78e2c, /*   58 */
    0x1f24db4e, /*   59 */
    0x1f51a620, /*   60 */
    0x1f7df23c, /*   61 */
    0x1fa9c314, /*   62 */
    0x1fd51bf2, /*   63 */
    0x20000000, /*   64 */
    0x202a7244, /*   65 */
    0x205475a6, /*   66 */
    0x207e0cee, /*   67 */
    0x20a73aca, /*   68 */
    0x20d001cc, /*   69 */
    0x20f8646d, /*   70 */
    0x2120650e, /*   71 */
    0x214805fa, /*   72 */
    0x216f4963, /*   73 */
    0x2196316c, /*   74 */
    0x21bcc020, /*   75 */
    0x21e2f77a, /*   76 */
    0x2208d961, /*   77 */
    0x222e67ad, /*   78 */
    0x2253a425, /*   79 */
    0x22789082, /*   80 */
    0x229d2e6e, /*   81 */
    0x22c17f82, /*   82 */
    0x22e5854f, /*   83 */
    0x23094155, /*   84 */
    0x232cb509, /*   85 */
    0x234fe1d5, /*   86 */
    0x2372c918, /*   87 */
    0x23956c26, /*   88 */
    0x23b7cc47, /*   89 */
    0x23d9eabb, /*   90 */
    0x23fbc8b9, /*   91 */
    0x241d676e, /*   92 */
    0x243ec7ff, /*   93 */
    0x245feb86, /*   94 */
    0x2480d319, /*   95 */
    0x24a17fc3, /*   96 */
    0x24c1f28b, /*   97 */
    0x24e22c6c, /*   98 */
    0x25022e5f, /*   99 */
    0x2521f954, /*  100 */
    0x25418e33, /*  101 */
    0x2560ede2, /*  102 */
    0x2580193e, /*  103 */
    0x259f111f, /*  104 */
    0x25bdd657, /*  105 */
    0x25dc69b4, /*  106 */
    0x25facbfe, /*  107 */
    0x2618fdf8, /*  108 */
    0x26370060, /*  109 */
    0x2654d3ef, /*  110 */
    0x2672795c, /*  111 */
    0x268ff156, /*  112 */
    0x26ad3c8a, /*  113 */
    0x26ca5ba2, /*  114 */
    0x26e74f41, /*  115 */
    0x27041808, /*  116 */
    0x2720b695, /*  117 */
    0x273d2b81, /*  118 */
    0x27597762, /*  119 */
    0x27759acb, /*  120 */
    0x2791964b, /*  121 */
    0x27ad6a6f, /*  122 */
    0x27c917c0, /*  123 */
    0x27e49ec5, /*  124 */
    0x28000000, /*  125 */
    0x281b3bf3, /*  126 */
    0x2836531b, /*  127 */
    0x285145f3, /*  128 */
    0x286c14f5, /*  129 */
    0x2886c096, /*  130 */
    0x28a1494b, /*  131 */
    0x28bbaf85, /*  132 */
    0x28d5f3b3, /*  133 */
    0x28f01641, /*  134 */
    0x290a179b, /*  135 */
    0x2923f82a, /*  136 */
    0x293db854, /*  137 */
    0x2957587e, /*  138 */
    0x2970d90a, /*  139 */
    0x298a3a59, /*  140 */
    0x29a37cca, /*  141 */
    0x29bca0bb, /*  142 */
    0x29d5a687, /*  143 */
    0x29ee8e87, /*  144 */
    0x2a075914, /*  145 */
    0x2a200684, /*  146 */
    0x2a38972c, /*  147 */
    0x2a510b5f, /*  148 */
    0x2a696370, /*  149 */
    0x2a819fae, /*  150 */
    0x2a99c069, /*  151 */
    0x2ab1c5ed, /*  152 */
    0x2ac9b088, /*  153 */
    0x2ae18085, /*  154 */
    0x2af9362c, /*  155 */
    0x2b10d1c6, /*  156 */
    0x2b28539b, /*  157 */
    0x2b3fbbef, /*  158 */
    0x2b570b09, /*  159 */
    0x2b6e412b, /*  160 */
    0x2b855e97, /*  161 */
    0x2b9c6390, /*  162 */
    0x2bb35056, /*  163 */
    0x2bca2527, /*  164 */
    0x2be0e242, /*  165 */
    0x2bf787e4, /*  166 */
    0x2c0e1649, /*  167 */
    0x2c248dad, /*  168 */
    0x2c3aee4a, /*  169 */
    0x2c513859, /*  170 */
    0x2c676c13, /*  171 */
    0x2c7d89af, /*  172 */
    0x2c939164, /*  173 */
    0x2ca98368, /*  174 */
    0x2cbf5ff1, /*  175 */
    0x2cd52731, /*  176 */
    0x2cead95e, /*  177 */
    0x2d0076a9, /*  178 */
    0x2d15ff45, /*  179 */
    0x2d2b7363, /*  180 */
    0x2d40d332, /*  181 */
    0x2d561ee4, /*  182 */
    0x2d6b56a7, /*  183 */
    0x2d807aaa, /*  184 */
    0x2d958b19, /*  185 */
    0x2daa8823, /*  186 */
    0x2dbf71f4, /*  187 */
    0x2dd448b7, /*  188 */
    0x2de90c98, /*  189 */
    0x2dfdbdc0, /*  190 */
    0x2e125c5c, /*  191 */
    0x2e26e892, /*  192 */
    0x2e3b628d, /*  193 */
    0x2e4fca75, /*  194 */
    0x2e642070, /*  195 */
    0x2e7864a8, /*  196 */
    0x2e8c9741, /*  197 */
    0x2ea0b862, /*  198 */
    0x2eb4c831, /*  199 */
    0x2ec8c6d3, /*  200 */
    0x2edcb46c, /*  201 */
    0x2ef09121, /*  202 */
    0x2f045d14, /*  203 */
    0x2f18186a, /*  204 */
    0x2f2bc345, /*  205 */
    0x2f3f5dc7, /*  206 */
    0x2f52e812, /*  207 */
    0x2f666247, /*  208 */
    0x2f79cc88, /*  209 */
    0x2f8d26f4, /*  210 */
    0x2fa071ac, /*  211 */
    0x2fb3acd0, /*  212 */
    0x2fc6d87f, /*  213 */
    0x2fd9f4d7, /*  214 */
    0x2fed01f8, /*  215 */
    0x30000000, /*  216 */
    0x3012ef0c, /*  217 */
    0x3025cf39, /*  218 */
    0x3038a0a6, /*  219 */
    0x304b636d, /*  220 */
    0x305e17ad, /*  221 */
    0x3070bd81, /*  222 */
    0x30835504, /*  223 */
    0x3095de51, /*  224 */
    0x30a85985, /*  225 */
    0x30bac6b9, /*  226 */
    0x30cd2609, /*  227 */
    0x30df778d, /*  228 */
    0x30f1bb60, /*  229 */
    0x3103f19c, /*  230 */
    0x31161a59, /*  231 */
    0x312835b0, /*  232 */
    0x313a43ba, /*  233 */
    0x314c4490, /*  234 */
    0x315e3849, /*  235 */
    0x31701efd, /*  236 */
    0x3181f8c4, /*  237 */
    0x3193c5b4, /*  238 */
    0x31a585e6, /*  239 */
    0x31b7396f, /*  240 */
    0x31c8e066, /*  241 */
    0x31da7ae1, /*  242 */
    0x31ec08f6, /*  243 */
    0x31fd8abc, /*  244 */
    0x320f0047, /*  245 */
    0x322069ac, /*  246 */
    0x3231c702, /*  247 */
    0x3243185c, /*  248 */
    0x32545dcf, /*  249 */
    0x32659770, /*  250 */
    0x3276c552, /*  251 */
    0x3287e78a, /*  252 */
    0x3298fe2c, /*  253 */
    0x32aa094a, /*  254 */
    0x32bb08f9, /*  255 */
    0x32cbfd4a, /*  256 */
    0x32dce652, /*  257 */
    0x32edc423, /*  258 */
    0x32fe96d0, /*  259 */
    0x330f5e6a, /*  260 */
    0x33201b04, /*  261 */
    0x3330ccb0, /*  262 */
    0x33417380, /*  263 */
    0x33520f85, /*  264 */
    0x3362a0d0, /*  265 */
    0x33732774, /*  266 */
    0x3383a380, /*  267 */
    0x33941506, /*  268 */
    0x33a47c17, /*  269 */
    0x33b4d8c4, /*  270 */
    0x33c52b1b, /*  271 */
    0x33d5732f, /*  272 */
    0x33e5b10f, /*  273 */
    0x33f5e4ca, /*  274 */
    0x34060e71, /*  275 */
    0x34162e14, /*  276 */
    0x342643c1, /*  277 */
    0x34364f88, /*  278 */
    0x34465178, /*  279 */
    0x345649a1, /*  280 */
    0x34663810, /*  281 */
    0x34761cd6, /*  282 */
    0x3485f800, /*  283 */
    0x3495c99d, /*  284 */
    0x34a591bb, /*  285 */
    0x34b55069, /*  286 */
    0x34c505b4, /*  287 */
    0x34d4b1ab, /*  288 */
    0x34e4545b, /*  289 */
    0x34f3edd2, /*  290 */
    0x35037e1d, /*  291 */
    0x3513054b, /*  292 */
    0x35228367, /*  293 */
    0x3531f881, /*  294 */
    0x354164a3, /*  295 */
    0x3550c7dc, /*  296 */
    0x35602239, /*  297 */
    0x356f73c5, /*  298 */
    0x357ebc8e, /*  299 */
    0x358dfca0, /*  300 */
    0x359d3408, /*  301 */
    0x35ac62d1, /*  302 */
    0x35bb8908, /*  303 */
    0x35caa6b9, /*  304 */
    0x35d9bbf0, /*  305 */
    0x35e8c8b9, /*  306 */
    0x35f7cd20, /*  307 */
    0x3606c92f, /*  308 */
    0x3615bcf3, /*  309 */
    0x3624a878, /*  310 */
    0x36338bc8, /*  311 */
    0x364266ee, /*  312 */
    0x365139f6, /*  313 */
    0x366004ec, /*  314 */
    0x366ec7d9, /*  315 */
    0x367d82c9, /*  316 */
    0x368c35c6, /*  317 */
    0x369ae0dc, /*  318 */
    0x36a98414, /*  319 */
    0x36b81f7a, /*  320 */
    0x36c6b317, /*  321 */
    0x36d53ef7, /*  322 */
    0x36e3c323, /*  323 */
    0x36f23fa5, /*  324 */
    0x3700b488, /*  325 */
    0x370f21d5, /*  326 */
    0x371d8797, /*  327 */
    0x372be5d7, /*  328 */
    0x373a3ca0, /*  329 */
    0x37488bf9, /*  330 */
    0x3756d3ef, /*  331 */
    0x37651489, /*  332 */
    0x37734dd1, /*  333 */
    0x37817fd1, /*  334 */
    0x378faa92, /*  335 */
    0x379dce1d, /*  336 */
    0x37abea7c, /*  337 */
    0x37b9ffb7, /*  338 */
    0x37c80dd7, /*  339 */
    0x37d614e6, /*  340 */
    0x37e414ec, /*  341 */
    0x37f20df1, /*  342 */
    0x38000000, /*  343 */
    0x380deb20, /*  344 */
    0x381bcf5a, /*  345 */
    0x3829acb6, /*  346 */
    0x3837833d, /*  347 */
    0x384552f8, /*  348 */
    0x38531bee, /*  349 */
    0x3860de28, /*  350 */
    0x386e99af, /*  351 */
    0x387c4e89, /*  352 */
    0x3889fcc0, /*  353 */
    0x3897a45b, /*  354 */
    0x38a54563, /*  355 */
    0x38b2dfdf, /*  356 */
    0x38c073d7, /*  357 */
    0x38ce0152, /*  358 */
    0x38db885a, /*  359 */
    0x38e908f4, /*  360 */
    0x38f68329, /*  361 */
    0x3903f701, /*  362 */
    0x39116483, /*  363 */
    0x391ecbb6, /*  364 */
    0x392c2ca1, /*  365 */
    0x3939874d, /*  366 */
    0x3946dbc0, /*  367 */
    0x39542a01, /*  368 */
    0x39617218, /*  369 */
    0x396eb40c, /*  370 */
    0x397befe4, /*  371 */
    0x398925a7, /*  372 */
    0x3996555c, /*  373 */
    0x39a37f09, /*  374 */
    0x39b0a2b7, /*  375 */
    0x39bdc06a, /*  376 */
    0x39cad82b, /*  377 */
    0x39d7ea01, /*  378 */
    0x39e4f5f0, /*  379 */
    0x39f1fc01, /*  380 */
    0x39fefc3a, /*  381 */
    0x3a0bf6a2, /*  382 */
    0x3a18eb3e, /*  383 */
    0x3a25da16, /*  384 */
    0x3a32c32f, /*  385 */
    0x3a3fa691, /*  386 */
    0x3a4c8441, /*  387 */
    0x3a595c46, /*  388 */
    0x3a662ea6, /*  389 */
    0x3a72fb67, /*  390 */
    0x3a7fc28f, /*  391 */
    0x3a8c8425, /*  392 */
    0x3a99402e, /*  393 */
    0x3aa5f6b1, /*  394 */
    0x3ab2a7b3, /*  395 */
    0x3abf533a, /*  396 */
    0x3acbf94d, /*  397 */
    0x3ad899f1, /*  398 */
    0x3ae5352c, /*  399 */
    0x3af1cb03, /*  400 */
    0x3afe5b7d, /*  401 */
    0x3b0ae6a0, /*  402 */
    0x3b176c70, /*  403 */
    0x3b23ecf3, /*  404 */
    0x3b306830, /*  405 */
    0x3b3cde2c, /*  406 */
    0x3b494eeb, /*  407 */
    0x3b55ba74, /*  408 */
    0x3b6220cc, /*  409 */
    0x3b6e81f9, /*  410 */
    0x3b7ade00, /*  411 */
    0x3b8734e5, /*  412 */
    0x3b9386b0, /*  413 */
    0x3b9fd364, /*  414 */
    0x3bac1b07, /*  415 */
    0x3bb85d9e, /*  416 */
    0x3bc49b2f, /*  417 */
    0x3bd0d3be, /*  418 */
    0x3bdd0751, /*  419 */
    0x3be935ed, /*  420 */
    0x3bf55f97, /*  421 */
    0x3c018453, /*  422 */
    0x3c0da427, /*  423 */
    0x3c19bf17, /*  424 */
    0x3c25d52a, /*  425 */
    0x3c31e662, /*  426 */
    0x3c3df2c6, /*  427 */
    0x3c49fa5b, /*  428 */
    0x3c55fd24, /*  429 */
    0x3c61fb27, /*  430 */
    0x3c6df468, /*  431 */
    0x3c79e8ed, /*  432 */
    0x3c85d8b9, /*  433 */
    0x3c91c3d2, /*  434 */
    0x3c9daa3c, /*  435 */
    0x3ca98bfc, /*  436 */
    0x3cb56915, /*  437 */
    0x3cc1418e, /*  438 */
    0x3ccd156a, /*  439 */
    0x3cd8e4ae, /*  440 */
    0x3ce4af5e, /*  441 */
    0x3cf0757f, /*  442 */
    0x3cfc3714, /*  443 */
    0x3d07f423, /*  444 */
    0x3d13acb0, /*  445 */
    0x3d1f60bf, /*  446 */
    0x3d2b1055, /*  447 */
    0x3d36bb75, /*  448 */
    0x3d426224, /*  449 */
    0x3d4e0466, /*  450 */
    0x3d59a23f, /*  451 */
    0x3d653bb4, /*  452 */
    0x3d70d0c8, /*  453 */
    0x3d7c6180, /*  454 */
    0x3d87ede0, /*  455 */
    0x3d9375ec, /*  456 */
    0x3d9ef9a8, /*  457 */
    0x3daa7918, /*  458 */
    0x3db5f43f, /*  459 */
    0x3dc16b23, /*  460 */
    0x3dccddc7, /*  461 */
    0x3dd84c2e, /*  462 */
    0x3de3b65d, /*  463 */
    0x3def1c58, /*  464 */
    0x3dfa7e22, /*  465 */
    0x3e05dbc0, /*  466 */
    0x3e113535, /*  467 */
    0x3e1c8a85, /*  468 */
    0x3e27dbb3, /*  469 */
    0x3e3328c4, /*  470 */
    0x3e3e71bb, /*  471 */
    0x3e49b69c, /*  472 */
    0x3e54f76b, /*  473 */
    0x3e60342b, /*  474 */
    0x3e6b6ce0, /*  475 */
    0x3e76a18d, /*  476 */
    0x3e81d237, /*  477 */
    0x3e8cfee0, /*  478 */
    0x3e98278d, /*  479 */
    0x3ea34c40, /*  480 */
    0x3eae6cfe, /*  481 */
    0x3eb989ca, /*  482 */
    0x3ec4a2a8, /*  483 */
    0x3ecfb79a, /*  484 */
    0x3edac8a5, /*  485 */
    0x3ee5d5cb, /*  486 */
    0x3ef0df10, /*  487 */
    0x3efbe478, /*  488 */
    0x3f06e606, /*  489 */
    0x3f11e3be, /*  490 */
    0x3f1cdda2, /*  491 */
    0x3f27d3b6, /*  492 */
    0x3f32c5fd, /*  493 */
    0x3f3db47b, /*  494 */
    0x3f489f32, /*  495 */
    0x3f538627, /*  496 */
    0x3f5e695c, /*  497 */
    0x3f6948d5, /*  498 */
    0x3f742494, /*  499 */
    0x3f7efc9d, /*  500 */
    0x3f89d0f3, /*  501 */
    0x3f94a19a, /*  502 */
    0x3f9f6e94, /*  503 */
    0x3faa37e4, /*  504 */
    0x3fb4fd8e, /*  505 */
    0x3fbfbf94, /*  506 */
    0x3fca7dfb, /*  507 */
    0x3fd538c4, /*  508 */
    0x3fdfeff3, /*  509 */
    0x3feaa38a, /*  510 */
    0x3ff5538e, /*  511 */
    0x40000000, /*  512 */
    0x400aa8e4, /*  513 */
    0x40154e3d, /*  514 */
    0x401ff00d, /*  515 */
    0x402a8e58, /*  516 */
    0x40352921, /*  517 */
    0x403fc06a, /*  518 */
    0x404a5436, /*  519 */
    0x4054e488, /*  520 */
    0x405f7164, /*  521 */
    0x4069facb, /*  522 */
    0x407480c1, /*  523 */
    0x407f0348, /*  524 */
    0x40898264, /*  525 */
    0x4093fe16, /*  526 */
    0x409e7663, /*  527 */
    0x40a8eb4c, /*  528 */
    0x40b35cd4, /*  529 */
    0x40bdcafe, /*  530 */
    0x40c835cd, /*  531 */
    0x40d29d43, /*  532 */
    0x40dd0164, /*  533 */
    0x40e76231, /*  534 */
    0x40f1bfae, /*  535 */
    0x40fc19dc, /*  536 */
    0x410670c0, /*  537 */
    0x4110c45a, /*  538 */
    0x411b14af, /*  539 */
    0x412561c0, /*  540 */
    0x412fab90, /*  541 */
    0x4139f222, /*  542 */
    0x41443578, /*  543 */
    0x414e7595, /*  544 */
    0x4158b27a, /*  545 */
    0x4162ec2c, /*  546 */
    0x416d22ac, /*  547 */
    0x417755fd, /*  548 */
    0x41818621, /*  549 */
    0x418bb31a, /*  550 */
    0x4195dcec, /*  551 */
    0x41a00399, /*  552 */
    0x41aa2722, /*  553 */
    0x41b4478b, /*  554 */
    0x41be64d6, /*  555 */
    0x41c87f05, /*  556 */
    0x41d2961a, /*  557 */
    0x41dcaa19, /*  558 */
    0x41e6bb03, /*  559 */
    0x41f0c8db, /*  560 */
    0x41fad3a3, /*  561 */
    0x4204db5d, /*  562 */
    0x420ee00c, /*  563 */
    0x4218e1b1, /*  564 */
    0x4222e051, /*  565 */
    0x422cdbeb, /*  566 */
    0x4236d484, /*  567 */
    0x4240ca1d, /*  568 */
    0x424abcb8, /*  569 */
    0x4254ac58, /*  570 */
    0x425e98fe, /*  571 */
    0x426882ae, /*  572 */
    0x42726969, /*  573 */
    0x427c4d31, /*  574 */
    0x42862e09, /*  575 */
    0x42900bf3, /*  576 */
    0x4299e6f1, /*  577 */
    0x42a3bf05, /*  578 */
    0x42ad9432, /*  579 */
    0x42b76678, /*  580 */
    0x42c135dc, /*  581 */
    0x42cb025e, /*  582 */
    0x42d4cc01, /*  583 */
    0x42de92c7, /*  584 */
    0x42e856b2, /*  585 */
    0x42f217c4, /*  586 */
    0x42fbd5ff, /*  587 */
    0x43059166, /*  588 */
    0x430f49f9, /*  589 */
    0x4318ffbc, /*  590 */
    0x4322b2b1, /*  591 */
    0x432c62d8, /*  592 */
    0x43361036, /*  593 */
    0x433fbaca, /*  594 */
    0x43496298, /*  595 */
    0x435307a2, /*  596 */
    0x435ca9e8, /*  597 */
    0x4366496e, /*  598 */
    0x436fe636, /*  599 */
    0x43798041, /*  600 */
    0x43831790, /*  601 */
    0x438cac28, /*  602 */
    0x43963e08, /*  603 */
    0x439fcd33, /*  604 */
    0x43a959ab, /*  605 */
    0x43b2e372, /*  606 */
    0x43bc6a89, /*  607 */
    0x43c5eef3, /*  608 */
    0x43cf70b2, /*  609 */
    0x43d8efc7, /*  610 */
    0x43e26c34, /*  611 */
    0x43ebe5fb, /*  612 */
    0x43f55d1e, /*  613 */
    0x43fed19f, /*  614 */
    0x44084380, /*  615 */
    0x4411b2c1, /*  616 */
    0x441b1f66, /*  617 */
    0x44248970, /*  618 */
    0x442df0e1, /*  619 */
    0x443755bb, /*  620 */
    0x4440b7fe, /*  621 */
    0x444a17ae, /*  622 */
    0x445374cc, /*  623 */
    0x445ccf5a, /*  624 */
    0x44662758, /*  625 */
    0x446f7ccb, /*  626 */
    0x4478cfb2, /*  627 */
    0x4482200f, /*  628 */
    0x448b6de5, /*  629 */
    0x4494b935, /*  630 */
    0x449e0201, /*  631 */
    0x44a7484b, /*  632 */
    0x44b08c13, /*  633 */
    0x44b9cd5d, /*  634 */
    0x44c30c29, /*  635 */
    0x44cc4879, /*  636 */
    0x44d5824f, /*  637 */
    0x44deb9ac, /*  638 */
    0x44e7ee93, /*  639 */
    0x44f12105, /*  640 */
    0x44fa5103, /*  641 */
    0x45037e8f, /*  642 */
    0x450ca9ab, /*  643 */
    0x4515d258, /*  644 */
    0x451ef899, /*  645 */
    0x45281c6e, /*  646 */
    0x45313dd8, /*  647 */
    0x453a5cdb, /*  648 */
    0x45437977, /*  649 */
    0x454c93ae, /*  650 */
    0x4555ab82, /*  651 */
    0x455ec0f3, /*  652 */
    0x4567d404, /*  653 */
    0x4570e4b7, /*  654 */
    0x4579f30c, /*  655 */
    0x4582ff05, /*  656 */
    0x458c08a4, /*  657 */
    0x45950fea, /*  658 */
    0x459e14d9, /*  659 */
    0x45a71773, /*  660 */
    0x45b017b8, /*  661 */
    0x45b915aa, /*  662 */
    0x45c2114c, /*  663 */
    0x45cb0a9e, /*  664 */
    0x45d401a1, /*  665 */
    0x45dcf658, /*  666 */
    0x45e5e8c4, /*  667 */
    0x45eed8e6, /*  668 */
    0x45f7c6c0, /*  669 */
    0x4600b253, /*  670 */
    0x46099ba0, /*  671 */
    0x461282a9, /*  672 */
    0x461b6770, /*  673 */
    0x462449f6, /*  674 */
    0x462d2a3c, /*  675 */
    0x46360844, /*  676 */
    0x463ee40f, /*  677 */
    0x4647bd9f, /*  678 */
    0x465094f5, /*  679 */
    0x46596a12, /*  680 */
    0x46623cf8, /*  681 */
    0x466b0da8, /*  682 */
    0x4673dc24, /*  683 */
    0x467ca86c, /*  684 */
    0x46857283, /*  685 */
    0x468e3a69, /*  686 */
    0x46970021, /*  687 */
    0x469fc3ab, /*  688 */
    0x46a88509, /*  689 */
    0x46b1443b, /*  690 */
    0x46ba0144, /*  691 */
    0x46c2bc25, /*  692 */
    0x46cb74df, /*  693 */
    0x46d42b74, /*  694 */
    0x46dcdfe4, /*  695 */
    0x46e59231, /*  696 */
    0x46ee425c, /*  697 */
    0x46f6f068, /*  698 */
    0x46ff9c54, /*  699 */
    0x47084622, /*  700 */
    0x4710edd4, /*  701 */
    0x4719936b, /*  702 */
    0x472236e7, /*  703 */
    0x472ad84b, /*  704 */
    0x47337798, /*  705 */
    0x473c14cf, /*  706 */
    0x4744aff1, /*  707 */
    0x474d48ff, /*  708 */
    0x4755dffb, /*  709 */
    0x475e74e6, /*  710 */
    0x476707c1, /*  711 */
    0x476f988e, /*  712 */
    0x4778274d, /*  713 */
    0x4780b400, /*  714 */
    0x47893ea8, /*  715 */
    0x4791c746, /*  716 */
    0x479a4ddc, /*  717 */
    0x47a2d26b, /*  718 */
    0x47ab54f3, /*  719 */
    0x47b3d577, /*  720 */
    0x47bc53f7, /*  721 */
    0x47c4d074, /*  722 */
    0x47cd4af0, /*  723 */
    0x47d5c36c, /*  724 */
    0x47de39e9, /*  725 */
    0x47e6ae69, /*  726 */
    0x47ef20ec, /*  727 */
    0x47f79173, /*  728 */
    0x48000000, /*  729 */
    0x48086c94, /*  730 */
    0x4810d730, /*  731 */
    0x48193fd5, /*  732 */
    0x4821a685, /*  733 */
    0x482a0b40, /*  734 */
    0x48326e07, /*  735 */
    0x483acedd, /*  736 */
    0x48432dc1, /*  737 */
    0x484b8ab5, /*  738 */
    0x4853e5bb, /*  739 */
    0x485c3ed2, /*  740 */
    0x486495fd, /*  741 */
    0x486ceb3c, /*  742 */
    0x48753e91, /*  743 */
    0x487d8ffd, /*  744 */
    0x4885df80, /*  745 */
    0x488e2d1d, /*  746 */
    0x489678d3, /*  747 */
    0x489ec2a4, /*  748 */
    0x48a70a91, /*  749 */
    0x48af509b, /*  750 */
    0x48b794c4, /*  751 */
    0x48bfd70c, /*  752 */
    0x48c81774, /*  753 */
    0x48d055fe, /*  754 */
    0x48d892aa, /*  755 */
    0x48e0cd7a, /*  756 */
    0x48e9066e, /*  757 */
    0x48f13d88, /*  758 */
    0x48f972c9, /*  759 */
    0x4901a632, /*  760 */
    0x4909d7c3, /*  761 */
    0x4912077e, /*  762 */
    0x491a3564, /*  763 */
    0x49226175, /*  764 */
    0x492a8bb4, /*  765 */
    0x4932b420, /*  766 */
    0x493adabc, /*  767 */
    0x4942ff87, /*  768 */
    0x494b2283, /*  769 */
    0x495343b1, /*  770 */
    0x495b6312, /*  771 */
    0x496380a7, /*  772 */
    0x496b9c71, /*  773 */
    0x4973b670, /*  774 */
    0x497bcea7, /*  775 */
    0x4983e515, /*  776 */
    0x498bf9bc, /*  777 */
    0x49940c9e, /*  778 */
    0x499c1db9, /*  779 */
    0x49a42d11, /*  780 */
    0x49ac3aa5, /*  781 */
    0x49b44677, /*  782 */
    0x49bc5088, /*  783 */
    0x49c458d8, /*  784 */
    0x49cc5f69, /*  785 */
    0x49d4643c, /*  786 */
    0x49dc6750, /*  787 */
    0x49e468a9, /*  788 */
    0x49ec6845, /*  789 */
    0x49f46627, /*  790 */
    0x49fc624f, /*  791 */
    0x4a045cbe, /*  792 */
    0x4a0c5575, /*  793 */
    0x4a144c76, /*  794 */
    0x4a1c41c0, /*  795 */
    0x4a243555, /*  796 */
    0x4a2c2735, /*  797 */
    0x4a341763, /*  798 */
    0x4a3c05de, /*  799 */
    0x4a43f2a7, /*  800 */
    0x4a4bddc0, /*  801 */
    0x4a53c729, /*  802 */
    0x4a5baee3, /*  803 */
    0x4a6394ef, /*  804 */
    0x4a6b794f, /*  805 */
    0x4a735c02, /*  806 */
    0x4a7b3d09, /*  807 */
    0x4a831c67, /*  808 */
    0x4a8afa1b, /*  809 */
    0x4a92d626, /*  810 */
    0x4a9ab089, /*  811 */
    0x4aa28946, /*  812 */
    0x4aaa605d, /*  813 */
    0x4ab235ce, /*  814 */
    0x4aba099b, /*  815 */
    0x4ac1dbc5, /*  816 */
    0x4ac9ac4c, /*  817 */
    0x4ad17b31, /*  818 */
    0x4ad94876, /*  819 */
    0x4ae1141a, /*  820 */
    0x4ae8de1f, /*  821 */
    0x4af0a686, /*  822 */
    0x4af86d50, /*  823 */
    0x4b00327d, /*  824 */
    0x4b07f60d, /*  825 */
    0x4b0fb803, /*  826 */
    0x4b17785f, /*  827 */
    0x4b1f3722, /*  828 */
    0x4b26f44b, /*  829 */
    0x4b2eafde, /*  830 */
    0x4b3669d9, /*  831 */
    0x4b3e223e, /*  832 */
    0x4b45d90e, /*  833 */
    0x4b4d8e4a, /*  834 */
    0x4b5541f2, /*  835 */
    0x4b5cf407, /*  836 */
    0x4b64a48a, /*  837 */
    0x4b6c537c, /*  838 */
    0x4b7400dd, /*  839 */
    0x4b7bacaf, /*  840 */
    0x4b8356f2, /*  841 */
    0x4b8affa7, /*  842 */
    0x4b92a6ce, /*  843 */
    0x4b9a4c69, /*  844 */
    0x4ba1f079, /*  845 */
    0x4ba992fd, /*  846 */
    0x4bb133f8, /*  847 */
    0x4bb8d369, /*  848 */
    0x4bc07151, /*  849 */
    0x4bc80db2, /*  850 */
    0x4bcfa88c, /*  851 */
    0x4bd741df, /*  852 */
    0x4bded9ad, /*  853 */
    0x4be66ff6, /*  854 */
    0x4bee04bb, /*  855 */
    0x4bf597fc, /*  856 */
    0x4bfd29bc, /*  857 */
    0x4c04b9f9, /*  858 */
    0x4c0c48b6, /*  859 */
    0x4c13d5f2, /*  860 */
    0x4c1b61af, /*  861 */
    0x4c22ebed, /*  862 */
    0x4c2a74ad, /*  863 */
    0x4c31fbf0, /*  864 */
    0x4c3981b6, /*  865 */
    0x4c410600, /*  866 */
    0x4c4888d0, /*  867 */
    0x4c500a25, /*  868 */
    0x4c578a00, /*  869 */
    0x4c5f0862, /*  870 */
    0x4c66854c, /*  871 */
    0x4c6e00bf, /*  872 */
    0x4c757abb, /*  873 */
    0x4c7cf341, /*  874 */
    0x4c846a52, /*  875 */
    0x4c8bdfee, /*  876 */
    0x4c935416, /*  877 */
    0x4c9ac6cb, /*  878 */
    0x4ca2380e, /*  879 */
    0x4ca9a7de, /*  880 */
    0x4cb1163e, /*  881 */
    0x4cb8832d, /*  882 */
    0x4cbfeead, /*  883 */
    0x4cc758bd, /*  884 */
    0x4ccec15f, /*  885 */
    0x4cd62894, /*  886 */
    0x4cdd8e5c, /*  887 */
    0x4ce4f2b7, /*  888 */
    0x4cec55a7, /*  889 */
    0x4cf3b72c, /*  890 */
    0x4cfb1747, /*  891 */
    0x4d0275f8, /*  892 */
    0x4d09d340, /*  893 */
    0x4d112f21, /*  894 */
    0x4d188999, /*  895 */
    0x4d1fe2ab, /*  896 */
    0x4d273a57, /*  897 */
    0x4d2e909d, /*  898 */
    0x4d35e57f, /*  899 */
    0x4d3d38fc, /*  900 */
    0x4d448b16, /*  901 */
    0x4d4bdbcd, /*  902 */
    0x4d532b21, /*  903 */
    0x4d5a7914, /*  904 */
    0x4d61c5a7, /*  905 */
    0x4d6910d9, /*  906 */
    0x4d705aab, /*  907 */
    0x4d77a31e, /*  908 */
    0x4d7eea34, /*  909 */
    0x4d862feb, /*  910 */
    0x4d8d7445, /*  911 */
    0x4d94b743, /*  912 */
    0x4d9bf8e6, /*  913 */
    0x4da3392d, /*  914 */
    0x4daa7819, /*  915 */
    0x4db1b5ac, /*  916 */
    0x4db8f1e6, /*  917 */
    0x4dc02cc7, /*  918 */
    0x4dc76650, /*  919 */
    0x4dce9e81, /*  920 */
    0x4dd5d55c, /*  921 */
    0x4ddd0ae1, /*  922 */
    0x4de43f10, /*  923 */
    0x4deb71eb, /*  924 */
    0x4df2a371, /*  925 */
    0x4df9d3a3, /*  926 */
    0x4e010283, /*  927 */
    0x4e083010, /*  928 */
    0x4e0f5c4b, /*  929 */
    0x4e168735, /*  930 */
    0x4e1db0cf, /*  931 */
    0x4e24d918, /*  932 */
    0x4e2c0012, /*  933 */
    0x4e3325bd, /*  934 */
    0x4e3a4a1a, /*  935 */
    0x4e416d2a, /*  936 */
    0x4e488eec, /*  937 */
    0x4e4faf62, /*  938 */
    0x4e56ce8c, /*  939 */
    0x4e5dec6b, /*  940 */
    0x4e6508ff, /*  941 */
    0x4e6c2449, /*  942 */
    0x4e733e4a, /*  943 */
    0x4e7a5702, /*  944 */
    0x4e816e71, /*  945 */
    0x4e888498, /*  946 */
    0x4e8f9979, /*  947 */
    0x4e96ad13, /*  948 */
    0x4e9dbf67, /*  949 */
    0x4ea4d075, /*  950 */
    0x4eabe03e, /*  951 */
    0x4eb2eec4, /*  952 */
    0x4eb9fc05, /*  953 */
    0x4ec10803, /*  954 */
    0x4ec812bf, /*  955 */
    0x4ecf1c39, /*  956 */
    0x4ed62471, /*  957 */
    0x4edd2b68, /*  958 */
    0x4ee4311f, /*  959 */
    0x4eeb3596, /*  960 */
    0x4ef238cd, /*  961 */
    0x4ef93ac6, /*  962 */
    0x4f003b81, /*  963 */
    0x4f073afe, /*  964 */
    0x4f0e393f, /*  965 */
    0x4f153642, /*  966 */
    0x4f1c320a, /*  967 */
    0x4f232c96, /*  968 */
    0x4f2a25e8, /*  969 */
    0x4f311dff, /*  970 */
    0x4f3814dc, /*  971 */
    0x4f3f0a80, /*  972 */
    0x4f45feeb, /*  973 */
    0x4f4cf21f, /*  974 */
    0x4f53e41a, /*  975 */
    0x4f5ad4de, /*  976 */
    0x4f61c46c, /*  977 */
    0x4f68b2c4, /*  978 */
    0x4f6f9fe6, /*  979 */
    0x4f768bd3, /*  980 */
    0x4f7d768c, /*  981 */
    0x4f846011, /*  982 */
    0x4f8b4862, /*  983 */
    0x4f922f81, /*  984 */
    0x4f99156d, /*  985 */
    0x4f9ffa27, /*  986 */
    0x4fa6ddb0, /*  987 */
    0x4fadc008, /*  988 */
    0x4fb4a12f, /*  989 */
    0x4fbb8127, /*  990 */
    0x4fc25ff0, /*  991 */
    0x4fc93d8a, /*  992 */
    0x4fd019f5, /*  993 */
    0x4fd6f533, /*  994 */
    0x4fddcf43, /*  995 */
    0x4fe4a827, /*  996 */
    0x4feb7fde, /*  997 */
    0x4ff2566a, /*  998 */
    0x4ff92bca, /*  999 */
    0x50000000, /* 1000 */
    0x5006d30b, /* 1001 */
    0x500da4ed, /* 1002 */
    0x501475a5, /* 1003 */
    0x501b4535, /* 1004 */
    0x5022139c, /* 1005 */
    0x5028e0dc, /* 1006 */
    0x502facf4, /* 1007 */
    0x503677e5, /* 1008 */
    0x503d41b0, /* 1009 */
    0x50440a55, /* 1010 */
    0x504ad1d5, /* 1011 */
    0x50519830, /* 1012 */
    0x50585d67, /* 1013 */
    0x505f217a, /* 1014 */
    0x5065e469, /* 1015 */
    0x506ca635, /* 1016 */
    0x507366df, /* 1017 */
    0x507a2667, /* 1018 */
    0x5080e4cd, /* 1019 */
    0x5087a212, /* 1020 */
    0x508e5e37, /* 1021 */
    0x5095193c, /* 1022 */
    0x509bd320, /* 1023 */
    0x50a28be6, /* 1024 */
};

/*----------------------------------------------------------------------------
; EXTERNAL FUNCTION REFERENCES
; Declare functions defined elsewhere and referenced in this module
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
; EXTERNAL GLOBAL STORE/BUFFER/POINTER REFERENCES
; Declare variables used in this module but defined elsewhere
----------------------------------------------------------------------------*/

