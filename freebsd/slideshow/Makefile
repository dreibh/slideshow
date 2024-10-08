# Created by: Thomas Dreibholz <thomas.dreibholz@gmail.com>
# $FreeBSD$

PORTNAME=	slideshow
DISTVERSION=	2.3.7
CATEGORIES=	net
MASTER_SITES=	https://www.nntb.no/~dreibh/slideshow/download/

MAINTAINER=	thomas.dreibholz@gmail.com
COMMENT=	SlideShow HTML image presentation

LICENSE=	GPLv3+
LICENSE_FILE=   ${WRKSRC}/COPYING

LIB_DEPENDS=    libGraphicsMagick.so:graphics/GraphicsMagick

USES=           cmake tar:xz

CMAKE_ARGS+=    -DCMAKE_INSTALL_MANDIR=${PREFIX}/man

.include <bsd.port.mk>
