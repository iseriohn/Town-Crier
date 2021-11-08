/* * Copyright (c) 2016-2018 by Cornell University.  All Rights Reserved.
 *
 * Permission to use the "TownCrier" software ("TownCrier"), officially
 * docketed at the Center for Technology Licensing at Cornell University
 * as D-7364, developed through research conducted at Cornell University,
 * and its associated copyrights solely for educational, research and
 * non-profit purposes without fee is hereby granted, provided that the
 * user agrees as follows:
 *
 * The permission granted herein is solely for the purpose of compiling
 * the TownCrier source code. No other rights to use TownCrier and its
 * associated copyrights for any other purpose are granted herein,
 * whether commercial or non-commercial.
 *
 * Those desiring to incorporate TownCrier software into commercial
 * products or use TownCrier and its associated copyrights for commercial
 * purposes must contact the Center for Technology Licensing at Cornell
 * University at 395 Pine Tree Road, Suite 310, Ithaca, NY 14850; email:
 * ctl-connect@cornell.edu; Tel: 607-254-4698; FAX: 607-254-5454 for a
 * commercial license.
 *
 * IN NO EVENT SHALL CORNELL UNIVERSITY BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF TOWNCRIER AND ITS
 * ASSOCIATED COPYRIGHTS, EVEN IF CORNELL UNIVERSITY MAY HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE WORK PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND CORNELL
 * UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.  CORNELL UNIVERSITY MAKES NO
 * REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER IMPLIED
 * OR EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, OR THAT THE USE
 * OF TOWNCRIER AND ITS ASSOCIATED COPYRIGHTS WILL NOT INFRINGE ANY
 * PATENT, TRADEMARK OR OTHER RIGHTS.
 *
 * TownCrier was developed with funding in part by the National Science
 * Foundation (NSF grants CNS-1314857, CNS-1330599, CNS-1453634,
 * CNS-1518765, CNS-1514261), a Packard Fellowship, a Sloan Fellowship,
 * Google Faculty Research Awards, and a VMWare Research Award.
 */

#ifndef TOWNCIRER_CONSTANTS_H
#define TOWNCIRER_CONSTANTS_H

#define PICOJSON_USE_INT64

#define TX_BUF_SIZE 2048

#define GASPRICE    0x0BA43B7400  // 50000000000
#define GASLIMIT    0x2DC6C0      // 3000000

#define TC_REQUEST_PAYLOAD_LIMIT 20480 // 2k payload limit

/* Define any events that Town Crier Supports here */
#define TYPE_LOOP_THROUGH   0x0
#define TYPE_FLIGHT_INS     0x1
#define TYPE_BITCOIN_FEE    0x2
#define TYPE_FINANCE_INFO   0x3
#define TYPE_UPS_TRACKING   0x4
#define TYPE_COINMARKET     0x5
#define TYPE_WEATHER        0x6
#define TYPE_CURRENT_VOTE   0x7
#define TYPE_ENCRYPT_TEST   0x8
#define TYPE_WOLFRAM		0x9

#define TYPE_FLIGHT_INS_ENC	0xA
#define TYPE_STEAM_EX       0xB

#define TYPE_SSA            0xC
#define TYPE_COINBASE       0xD

/* Possible return values to be used in Enclave-Relay Connections */
#define TC_SUCCESS          0x0
#define TC_INPUT_ERROR      0x1
#define TC_UNKNOWN_ERROR    0x2
#define TC_INTERNAL_ERROR   0x3
#define TC_INTERNAL_TEST    0x99  // reserved for internal test

/* definition for error flags */
#define TC_ERR_FLAG_NO_ERROR      0
#define TC_ERR_FLAG_INVALID_INPUT 1
#define TC_ERR_FLAG_INTERNAL_ERR  2

#endif
