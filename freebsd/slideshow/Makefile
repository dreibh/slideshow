# Created by: Thomas Dreibholz <thomas.dreibholz@gmail.com>
# $FreeBSD$

PORTNAME=	slideshow
PORTVERSION=	2.3.6
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
