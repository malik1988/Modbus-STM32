#ifndef CONFIG_H
#define CONFIG_H

/* #undef HAVE_ARPA_INET_H */
/* #undef HAVE_BYTESWAP_H */
#define HAVE_ERRNO_H
//#define HAVE_FCNTL_H
#define HAVE_LIMITS_H
/* #undef HAVE_LINUX_SERIAL_H */
/* #undef HAVE_NETDB_H */
/* #undef HAVE_NETINET_IN_H */
/* #undef HAVE_NETINET_TCP_H */
/* #undef HAVE_SYS_IOCTL_H */
/* #undef HAVE_SYS_PARAMS_H */
/* #undef HAVE_SYS_SOCKET_H */
/* #undef HAVE_SYS_TIME_H */
#define HAVE_SYS_TYPES_H
/* #undef HAVE_TERMIOS_H */
#define HAVE_TIME_H
/* #undef HAVE_UNISTD_H */

/* #undef HAVE_ACCEPT4 */
/* #undef HAVE_FORK */
/* #undef HAVE_GETADDRINFO */
/* #undef HAVE_GETTIMEOFDAY */
/* #undef HAVE_INET_NTOA */
#define HAVE_MALLOC
#define HAVE_MEMSET
/* #undef HAVE_SELECT */
/* #undef HAVE_SOCKET */
#define HAVE_STRERROR
/* #undef HAVE_STRLCPY */

/* #undef HAVE_TIOCRS485 */
/* #undef HAVE_TIOCM_RTS */

#ifdef HAVE_TIOCM_RTS
#define HAVE_DECL_TIOCM_RTS 1
#else
#define HAVE_DECL_TIOCM_RTS 0
#endif

#ifdef HAVE_TIOCRS485
#define HAVE_DECL_TIOCSRS485 1
#else
#define HAVE_DECL_TIOCSRS485 0
#endif

#endif
