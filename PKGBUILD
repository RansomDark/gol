# Maintainer: RansomDark arch@localdomain>
pkgname=gol
pkgver=1.0
pkgrel=1
pkgdesc="Terminal-based Conway's Game of Life implementation"
arch=('x86_64' 'i686')
url="https://github.com/RansomDark/gol"
license=('MIT')
depends=('ncurses')
makedepends=('git')
source=("$pkgname-$pkgver.tar.gz")
md5sums=("47a7b0732aad9febce5318e4f99c9ccd")

build() {
  cd "$srcdir/$pkgname-$pkgver"
  make
}

package() {
  cd "$srcdir/$pkgname-$pkgver"
  
  install -d "$pkgdir/usr/share/gol/presets"
	cp -r presets/* "$pkgdir/usr/share/gol/presets/"
  
  install -Dm755 gol "$pkgdir/usr/bin/gol"
  
  chmod -R 644 "$pkgdir/usr/share/gol/presets/"*
}
