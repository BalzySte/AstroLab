
c  FITSIOWRAP is copyright Benjamin Weiner, free, and GPL licensed.
c  Do not sell.  Do not redistribute without this notice.  
c  Do not fold, spindle, or mutilate.  Do ask "When was the last
c  time you saw a card that said 'Do not fold, spindle, or mutilate'?"

c  requires header file "nmax.f" which should define max row length, e.g.
c    parameter(nmax=2048)
c  this is a kludge.  compile as f77 -c -o fitsiowrap.o fitsiowrap.f

c  These routines are intended to mimic the IMFORT routines of the
c  same names but use FITSIO routines to access FITS files, so that
c  you can use a program written to work with Imfort on fits files
c  by just linking against this file and the fitsio library.

c  The routines do not duplicate every imfort routine, and there
c  are some bugs.  Also, the duplication of imfort routines is
c  not totally verbatim.  In particular, the error codes and messages
c  are different, and (important) the handling of returning a nonzero
c  error code is not very consistent (routines like imopen do return
c  a nonzero error code if they fail, since opening an image is the
c  most likely place for an IMFORT user to actually check the error code).
c  There may be other oddities in e.g. the handling of header formats.

c  These caveats aside, the routines should handle most
c  things a rational person should want to do to a
c  2-D real image.  Anything not included is by definition irrational.

c  written by Ben Weiner, bjw@ociw.edu
c  some original basic routines were written by Dan Kelson

c   version of 5/26/99 -- a number of fixes, some routines
c       added to make daophot happy.  
c   5/26/99 - Added imaddk, imdelk, impkw[bcird], fixed imopnc
c        added Povi's fixes for imhcpy

c   9/22/99 - Known issue: When opening an image, if the 
c      filename variable is short, eg character*40 instead
c      of character*80, and you type the name "ccd030.fits"
c      instead of "ccd030", a seg fault occurs.  I do not know
c      why this happens.

c   4/27/00 - fixed a bunch of broken write/format statements 
c      in the error reporting in exftop, imcrea, etc.
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  added 11/11/98 bjw.  

      subroutine imopen(name, ac, im, ierr)
      
c      character*72 name
      character name*(*)
c      character*76 name2
      integer ac, im, ierr, block, ac2

      ierr = 0

      call ftcmsg()

      if ( ac.eq.1) ac2=0
      if ( ac.eq.3) ac2=1

      call ftgiou( im, ierr)
c      call doerr(ierr)
      call exftop( im, name, ac2, block, ierr)

c  preserve the value of ierr.  This is necessary because
c  doerr clears it, but the calling program may check for
c  nonzero ierr to see if it successfully opened the file.
c   Note: this means the calling program should clear ierr
c   before passing it back into another fitsio routine,
c   otherwise that routine won't do anything!  see the fitsio manual
      itmp = ierr
      if (ierr .ne. 0) write(*,'("fitsio: could not open ",a)') name
      call doerr(ierr)
      ierr = itmp

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  added 11/11/98 bjw.  

      subroutine imclos(im, ierr)

      integer ierr, im

      ierr = 0
      call ftclos(im, ierr)
c      call doerr(ierr)
      call ftfiou(im, ierr)
      call doerr(ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  added 11/11/98 bjw.  

      subroutine imopnc( name, im1, im2, ierr)

      character name*(*)
      integer im1,im2,ierr,jerr
      integer isize(7), idim, itype

      call imgsiz( im1, isize, idim, itype, ierr)

      call imcrea( name, isize, idim, itype, ierr)

      call imopen( name, 3, im2, ierr)
c  if ierr is nonzero, want to return it, so preserve it
      jerr=ierr
      call imhcpy( im1, im2, ierr)
      ierr=jerr
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  modified to handle .fits file extension intelligently

      subroutine imdele( name, ierr)

      character name*(*)
      integer im, ierr, block

      ierr = 0
      call ftgiou( im, ierr)
c      call doerr(ierr)
      call exftop( im, name, 1, block, ierr)
c      call ftopen( im, name, 1, block, ierr)
      call ftdelt( im, ierr)
      call ftclos( im, ierr)
      call ftfiou(im, ierr)
      if (ierr .ne. 0 ) 
     $     write(*,'("fitsio: error deleting file ",a)') name
      call doerr(ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  modified to handle .fits file extension intelligently

      subroutine imcrea( name, axlen, naxis, dtype, ierr)

      character name*(*)
      integer axlen(7), naxis, dtype, ierr, bitpix, block, im

      ierr = 0
      bitpix = -32
      call ftgiou( im, ierr)
c      call doerr(ierr)
      call exftin( im, name, 0, ierr)
c      call ftinit( im, name, 0, ierr)
      jerr = ierr
      call doerr(ierr)
      if (jerr.ne.0) then
       call exftop( im, name, 1, block, ierr)
c       call ftopen( im, name, 1, block, ierr)
       write(6,'("fitsio: deleting file ",a)') name
       call ftdelt( im, ierr)
       write(6,'("fitsio: creating file ",a)') name
       call exftin( im, name, 0, ierr)
c       call ftinit( im, name, 0, ierr)
       if (ierr .ne. 0) 
     $      write (*,'("fitsio: error creating file ",a)') name
       call doerr(ierr)
      end if
      call ftphps( im, bitpix, naxis, axlen, ierr)
      call doerr(ierr)
      call ftclos( im, ierr)
      call doerr(ierr)
      call ftfiou( im, ierr)
      if (ierr .ne. 0) 
     $     write (*,'("fitsio: error creating file ",a)') name
      call doerr(ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  this is here to allow you to specify either "filename"
c  or "filename.fits", as imfort does with .imh - bjw

      subroutine exftop( im, name, ac2, block, ierr)

      character name*(*)
      character*80 name2
      integer im, ierr, block, ac2, iflag

      if (ierr .ne. 0) return
c  if a file exists with this name, we want it:
c  try opening, then add .fits extension if it fails
      call ftopen( im, name, ac2, block, ierr)
      if(ierr .ne. 0) then
         ierr = 0
         call addext(name,name2,'.fits',iflag)
         if (iflag .ne. 0) then
            call ftopen( im, name2, ac2, block, ierr)
         end if
      end if
c      call doerr(ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  this is here to allow you to specify either "filename"
c  or "filename.fits", as imfort does with .imh - bjw

      subroutine exftin( im, name, block, ierr)

      character name*(*)
      character*80 name2
      integer im, ierr, block

c  add .fits extension if it's not there already
      call addext(name,name2,'.fits',iflag)
      if (iflag .ne. 0) then
         call ftinit( im, name2, block, ierr)
      else
         call ftinit( im, name, block, ierr)
      end if
c      call doerr(ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  add an extension (i.e. .fits) if necessary and return
c  iflag=1 if it was added, iflag=0 otherwise

      subroutine addext(name1,name2,exten,iflag)

      character name1*(*)
      character name2*(*)
      character exten*(*)
      integer iflag

c  len1, len2 are lengths of the actual variables,
c  ind1 is length of the actual name
      iflag=0
      len1 = len(name1)
      len2 = len(name2)
c find the end of the word (first blank) in name1.  if the name1 array is
c full, the index() will return 0, so the blank is in length1 + 1.
c  set ind1 to be length of name (loc of blank - 1)
      ind1 = index(name1,' ') - 1
      if (ind1 .le. 0) ind1=len1
      lenext = len(exten)
c      write(*,*) len1,len2,lenext,ind1
      ipos = index(name1,exten)
      if (ipos .eq. 0) then
c  we didn't find the extension so add it
         if (ind1+lenext .gt. len2) then
            write (*,'("   Warning: name too long: ",a,a)')
     $           name1(1:ind1), exten(1:lenext)
         end if
         write (name2,'(a,a)') name1(1:ind1),exten(1:lenext)
c         write (*,'("addext: ",a)') name2
         iflag=1
      end if

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  fitsio does not make this easy...
      subroutine imrnam(oldnam, newnam, ierr)

      include "nmax.f"
      character oldnam*(*),newnam*(*)
      integer ierr
      integer im1, im2, idim, itype, isize(7)
      real array(nmax,nmax)

      call imopen(oldnam, 1, im1, ierr)
      if (ierr .ne. 0) then
         write(*,'("fitsio: could not open old image ",a)') oldnam
         go to 666
      end if
      call imopnc(newnam, im1, im2, ierr)
       if (ierr .ne. 0) then
         write(*,'("fitsio: could not open new image ",a)') newnam
         call imclos(im1, ierr)
         go to 666
      end if
      call imgsiz( im1, isize, idim, itype, ierr)
      i1=1
      i2=isize(1)
      j1=1
      j2=isize(2)
      call imgs2r( im1, array, i1, i2, j1, j2, ierr)
      call imps2r( im2, array, i1, i2, j1, j2, ierr)
      call imclos( im1, ierr)
      call imdele( oldnam, ierr)
      call imclos( im2, ierr)
      call doerr(ierr)

 666  continue
      return
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  I don't think there is any way to flush the image buffers
c  in fitsio (short of closing and reopening the image?
c  but this is non trivial since we need the image name to reopen)
c  Fortunately this routine should rarely be consequential.
      subroutine imflsh(im, ierr)

      integer im, ierr
      
      write(*,*) '  fitsio: warning: no imflsh - hopefully no problem'
      return
      end
      

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  added 11/11/98 bjw.

      subroutine imgsiz( im, isize, idim, itype, ierr)
      
      integer im,isize(7),idim,itype,ierr

      ierr = 0
      call ftgknj( im, 'NAXIS', 1, 7, isize, idim, ierr)
      call doerr(ierr)

c kludge - type everything as real
      itype = 6

      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  modified - bjw
c  is there an easier way?  can't tell from fitsio manual.
c  I don't think you can use ftcopy because that would copy
c  the data as well as the header.

      subroutine imhcpy( im1, im2, ierr)

      integer im1, im2
      character*80 card, comment
      integer nexist,nadd,ntmp

      ierr = 0
      call ftgkyj(im2, 'naxis', naxis, comment, ierr)
c      call doerr( ierr)

      if ( naxis.eq.1) js=5
      if ( naxis.eq.2) js=6

      ierr = 0
      do j=js,js+4
      call ftdrec(im2, 6, ierr)
c      call doerr( ierr)
      end do
  50      continue

c      write(6,*) 'copying header'

c  instead of looping to end, cleaner to use
c  ftghsp to find # of existing keywords
      call ftghsp( im1,nexist,ntmp,ierr)
      call ftghsp( im2,ntmp,nadd,ierr)
      if (nadd .ne. -1 .and. nexist .gt. ntmp+nadd) then
         write(*,*) 'fitsio: not enough space to copy header? '
c  do something about it?
      end if
      do i=1,nexist
         call ftgrec( im1, i, card, ierr)
         if ( ierr.ne.0) then
c  assume we've reached end of header
c            write (*,*) '  fitsio/imhcpy: error getting record',i,card(1:8)
            goto 10
         end if
         if ( card(1:8).eq.'SIMPLE  ') goto 5
         if ( card(1:8).eq.'BITPIX  ') goto 5
         if ( card(1:8).eq.'NAXIS   ') goto 5
         if ( card(1:8).eq.'NAXIS1  ') goto 5
         if ( card(1:8).eq.'NAXIS2  ') goto 5
         if ( card(1:8).eq.'BSCALE  ') goto 5
         if ( card(1:8).eq.'BZERO   ') goto 5
         if ( card(1:8).eq.'EXTEND  ') goto 5
         if ( card(1:8).eq.'UNSIGN  ') goto 5
c  test
         if (ichar(card(80:80)).eq.10) card(80:80)=' '
c         write(6,'(i2,i3,2x,a72)') ierr,i,card(1:72)
         call ftprec( im2, card, ierr)
c  Here it might be possible to run out of space in the header
c  of the second image.         
c         if ( ierr.ne.0) then
c        write (*,*) '  fitsio/imhcpy: error putting record',i,card(1:8)
c         end if
         call doerr( ierr)
 5       continue
      end do

 10   continue
      ierr = 0
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imacck( im, keyw, ierr)

      integer im, ierr
      character keyw*(*)
      character comment*80
      
      ierr = 0
      call ftgcrd( im, keyw, comment, ierr)      
c      write(6,*) keyw
c      write(6,*) comment
      itmp=ierr
      call doerr( ierr)
      ierr=itmp
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  this routine is somewhat redundant - using  imakwr, etc
c  is better.
      subroutine imaddk( im, keyw, itype, comment, ierr)
      integer im, itype, ierr
      character keyw*(*),comment*(*)
      character tval*80,tcomm*80

      call ftgkey(im, keyw, tval, tcomm, ierr)
      if (ierr .eq. 0) then
c  keyword already exists         
         ierr = 1
         return
      end if
      if (itype .eq. 1) then
         call ftpkyl( im, keyw, .true., comment, ierr)
         return
      else if (itype .eq. 2) then
         call ftpkys( im, keyw, ' ', comment, ierr)
         return
      else if (itype .eq. 3) then
c  there are no short integer headers in fitsio
         call ftpkyj( im, keyw, 0, comment, ierr)
         return
      else if (itype .eq. 4 .or. itype .eq. 5) then
         call ftpkyj( im, keyw, 0, comment, ierr)
         return
      else if (itype .eq. 6) then
         call ftpkye( im, keyw, 0.0, comment, ierr)
         return
      else if (itype .eq. 7) then
         call ftpkyd( im, keyw, 0.d0, comment, ierr)
         return
      end if

c  unknown type
      ierr=1
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine imdelk(im, keyw, ierr)
      integer im,ierr
      character keyw*(*)
      
      call ftdkey(im, keyw, ierr)
      return
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imakwb(im, keyw, bval, comment, ierr)

      integer im, ierr
      character keyw*(*), comment*(*)
      logical bval
      
      call ftukyl(im, keyw, bval, comment, ierr)
      call doerr(ierr)
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imakwc( im, keyw, value, null, ierr)

      integer im,ierr
      character keyw*(*), null*(*)
      character value*(*)

      call ftukys( im, keyw, value, null, ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c modified

      subroutine imakwi( im, keyw, value, null, ierr)

      integer im, ierr
      character keyw*(*), null*(*)
      integer value

      call ftukyj( im, keyw, value, null, ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  modified

      subroutine imakwr( im, keyw, value, null, ierr)

      integer im
      character keyw*(*), null*(*)
      real value

c      call ftukyf( im, keyw, value, null, ierr)
      call ftukye( im, keyw, value, 5, null, ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imakwd(im, keyw, value, comment, ierr)

      integer im, ierr
      character keyw*(*), comment*(*)
      double precision value 
      
      call ftukyd(im, keyw, value, 8, comment, ierr)
      call doerr(ierr)
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine impkwb(im, keyw, bval, ierr)

      integer im, ierr
      character keyw*(*)
      logical bval
      
      call ftmkyl(im, keyw, bval, '&', ierr)
      call doerr(ierr)
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine impkwc( im, keyw, value, ierr)

      integer im,ierr
      character keyw*(*)
      character value*(*)

      call ftmkys( im, keyw, value, '&', ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine impkwi( im, keyw, value, ierr)

      integer im, ierr
      character keyw*(*)
      integer value

      call ftmkyj( im, keyw, value, '&', ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine impkwr( im, keyw, value, ierr)

      integer im
      character keyw*(*)
      real value

      call ftmkye( im, keyw, value, 5, '&', ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine impkwd(im, keyw, value, ierr)

      integer im, ierr
      character keyw*(*)
      double precision value 
      
      call ftmkyd(im, keyw, value, 8, '&', ierr)
      call doerr(ierr)
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imgkwb(im, keyw, bval, ierr)

      integer im, ierr
      character keyw*(*), comment*80
      logical bval
      
      call ftgkyl(im, keyw, bval, comment, ierr)
      itmp=ierr
      call doerr(ierr)
      ierr=itmp
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imgkwc( im, keyw, value, ierr)

      integer im, ierr
      character keyw*(*)
      character*32 comment

      character*32 value

      ierr = 0
      call ftgkys(im, keyw, value, comment, ierr)
      itmp=ierr
      call doerr( ierr)
      ierr=itmp

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imgkwi( im, keyw, value, ierr)

      integer im, ierr
      character keyw*(*)
      character*32 comment
      integer value

      ierr = 0
      call ftgkyj(im, keyw, value, comment, ierr)
      itmp=ierr
      call doerr( ierr)
      ierr=itmp
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imgkwr( im, keyw, value, ierr)

      integer im, ierr
      character keyw*(*)
      character*32 comment
      real value

      ierr = 0
      call ftgkye(im, keyw, value, comment, ierr)
      itmp=ierr
      call doerr( ierr)
      ierr=itmp
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imgkwd(im, keyw, bval, ierr)

      integer im, ierr
      character keyw*(*), comment*80
      double precision value
      
      call ftgkyd(im, keyw, value, comment, ierr)
      itmp=ierr
      call doerr(ierr)
      ierr=itmp
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine impl2r(im, array, j, ierr)

      include "nmax.f"
      real array(nmax)
      character*32 comment
      integer naxis1, fpixel, ierr, im

      ierr = 0
      call ftgkyj(im, 'naxis1', naxis1, comment, ierr)
c      call doerr( ierr)

      fpixel = 1 + (j-1)*naxis1
      call ftppre( im, 0, fpixel, naxis1, array, ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imgl2r(im, array, j, ierr)

      include "nmax.f"
      real array(nmax)
      character*32 comment
      integer im, naxis1, fpixel, ierr
      logical flag

      ierr = 0
      call ftgkyj(im, 'naxis1', naxis1, comment, ierr)
c      call doerr( ierr)

      fpixel = 1 + (j-1)*naxis1
      call ftgpve( im, 0, fpixel, naxis1, 0.0,
     &            array, flag, ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c  changed order of arguments 1/3/99 - bjw
c  fixed to behave like the imfort routine

      subroutine imgs2r( im, array, i1, i2, j1, j2, ierr)

      include "nmax.f"

      real array(nmax, 2048)
c      integer naxis1, fpixel, lpixel
      integer naxis,naxes(2),incs(2)
      real nullval
      logical anyf
      integer group,fpixels(2),lpixels(2)

c      ierr = 0
      call ftgknj( im, 'NAXIS', 1, 7, naxes, naxis, ierr)
c      call doerr(ierr)
c      call ftgkyj(im, 'naxis1', naxis1, comment, ierr)
c      call doerr( ierr)

      group = 0
      fpixels(1) = i1
      fpixels(2) = j1
      lpixels(1) = i2
      lpixels(2) = j2
      incs(1) = 1
      incs(2) = 1
      nullval = 0.
      call ftgsve( im,group,naxis,naxes,fpixels,lpixels,
     $     incs,nullval,array,anyf,ierr)
      call doerr(ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c  this is the original version which does not actually do
c  what imgs2r does (imgs2r returns the subsection in Fortran
c  storage order, _without_ regard for the rows and columns 
c  of the array, and without padding to the beginning/end
c  of the image, etc. -- i.e. this routine
c  is somewhat more intelligent than the real imgs2r, but 
c  nominally relies on knowledge about the array/image size)

      subroutine imgs2r2( im, array, i1, i2, j1, j2, ierr)

      include "nmax.f"

      real array(nmax, 2048)
      integer naxis1, fpixel, lpixel
      character*80 comment

      ierr = 0
      call ftgkyj(im, 'naxis1', naxis1, comment, ierr)
      call doerr( ierr)

      do j=j1,j2
      jj = j-j1+1
      fpixel = i1 + (j1-1)*naxis1
      lpixel = fpixel + (i2-i1)-1
      call imgl2r( im, array(1,jj), j, ierr)
      end do

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine impl1r(im, array, ierr)

      include "nmax.f"
      real array(nmax)
      character*32 comment
      integer naxis1, fpixel, ierr, im

      ierr = 0
      call ftgkyj(im, 'naxis1', naxis1, comment, ierr)
c      call doerr( ierr)

      fpixel = 1
      call ftppre( im, 0, fpixel, naxis1, array, ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imgl1r(im, array, ierr)

      include "nmax.f"
      real array(nmax)
      character*32 comment
      integer im, naxis1, fpixel, ierr
      logical flag

      ierr = 0
      call ftgkyj(im, 'naxis1', naxis1, comment, ierr)
      call doerr( ierr)

      fpixel = 1
      call ftgpve( im, 0, fpixel, naxis1, 0.0,
     &            array, flag, ierr)
      call doerr( ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c  imps2r - fixed to behave like the imfort routine
      subroutine imps2r( im, array, i1, i2, j1, j2, ierr)

      include "nmax.f"

      integer im,i1,i2,j1,j2,ierr
      real array(nmax,2048)
      integer naxis,naxes(2)
      integer group,fpixels(2),lpixels(2)

      call ftgknj( im, 'NAXIS', 1, 7, naxes, naxis, ierr)
    
      group = 1
      fpixels(1) = i1
      fpixels(2) = j1
      lpixels(1) = i2
      lpixels(2) = j2
      call ftpsse( im,group,naxis,naxes,fpixels,lpixels,
     $     array,ierr)
      call doerr(ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c  this is the original version of imps2r - as with imgs2r2,
c  this does not emulate the actual imps2r routine; the actual
c  routine does not pad into the array.  (i.e. this routine
c  is somewhat more intelligent than the real imps2r)
      subroutine imps2r2( im, array, i1, i2, j1, j2, ierr)

      include "nmax.f"

c      real row(nmax)
      real array(nmax, 2048)
      integer naxis1, fpixel, lpixel
      character*80 comment

      ierr = 0
      call ftgkyj(im, 'naxis1', naxis1, comment, ierr)
      call doerr( ierr)

      do j=j1,j2
      jj = j-j1+1
      fpixel = i1 + (j1-1)*naxis1
      lpixel = fpixel + (i2-i1)-1
      call impl2r( im, array(1,jj), j, ierr)
      end do

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  just in case somebody actually tries to use this routine
c   there are no pixel files in fits-land, so this is irrelevant.
      subroutine imsdir(dummy)

      character*80 dummy

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine imgdir(dummy)

      character dummy*(*)

      write(dummy,'(a)') ''

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  The problem with this routine is that it clears the error
c  flag rather than returning it ...

      subroutine doerr( status)

      integer status
      character*80 error

      if ( status.ne.0) then
      call ftgerr( status, error)
      write(6,25) error
c  25      format(20x,a30)
  25      format(2x,'fitsio: ',a30)
      end if

      status = 0

      call ftcmsg()

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine errch(keyw, ierr)

      character keyw*(*)

      if ( ierr.ne.0) then
      call ftgerr( ierr, keyw)

      ierr = 0

      call ftcmsg()
      end if

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c return type info for keyword.  
      subroutine imtypk(im, keyw, itype, comm, ier)

      integer im,itype,ier
      character keyw*(*),value*80
      character comm*(*)
      character dtype*1

c  I couldn't immediately figure
c out how to do this with fitsio (Gaahh!!) so I will make
c it return the type as a string, since you can read
c e.g. real values out of a string
      call ftgkys(im, keyw, value, comm, ierr)
      call doerr(ierr)
      itype = 2

c  or do it this way?  I'm not clear on what ftdtyp really does
      call ftdtyp(value,dtype,ierr)
      call doerr(ierr)
c  convert to imfort numbers
      if (dtype(1:1) .eq. 'C') then
         itype = 2
      else if (dtype(1:1) .eq. 'L') then
         itype = 1
      else if (dtype(1:1) .eq. 'I') then
         itype = 4
      else if (dtype(1:1) .eq. 'F') then
         itype = 6
      end if

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine imemsg(ierr, errmsg)

      integer ierr
      character errmsg*(*)
      
      call ftgerr(ierr, errmsg)
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  Kelson's opening routine.  
      subroutine openit( name, ac, im)

      character*72 name
      integer ac, im, ierr, block, ac2

      ierr = 0

      call ftcmsg()

      if ( ac.eq.1) ac2=0
      if ( ac.eq.3) ac2=1

      call ftgiou( im, ierr)
c      call doerr(ierr)
      call ftopen( im, name, ac2, block, ierr)
      call doerr(ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c  Kelson's closing routine.
      subroutine closeit( im)

      integer ierr, im

      ierr = 0
      call ftgiou( im, ierr)
c      call doerr(ierr)
      call ftclos(im, ierr)
      call doerr(ierr)

      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
