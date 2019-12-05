# Created by: Thomas Dreibholz <dreibh@iem.uni-due.de>
# $FreeBSD$

PORTNAME=	slideshow
PORTVERSION=	2.3.2~rc2
CATEGORIES=	net
MASTER_SITES=	https://www.uni-due.de/~be0001/slideshow/download/

MAINTAINER=	dreibh@iem.uni-due.de
COMMENT=	SlideShow HTML image presentation

LICENSE=	GPLv3+
LICENSE_FILE=   ${WRKSRC}/COPYING

LIB_DEPENDS=    libGraphicsMagick.so:graphics/GraphicsMagick

USES=           cmake tar:xz

CMAKE_ARGS+=    -DCMAKE_INSTALL_MANDIR=${PREFIX}/man

.include <bsd.port.mk>
