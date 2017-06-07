      double precision function round ( x )

c*********************************************************************72
c
cc ROUND is called to add some noise to data.
c
c  from  * a practical guide to splines *  by c. de boor    
called in example 1  of chapter xiii
c
      implicit none

      double precision x,   flip,size
      common /rount/ size
      data flip /-1.0D+00/
      flip = -flip
      round = x + flip*size
                                        return
      end
