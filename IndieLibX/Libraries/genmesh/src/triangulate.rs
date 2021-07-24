use std::collections::VecDeque;

use crate::Polygon::{self, PolyQuad, PolyTri};
use crate::{Quad, Triangle};

/// Provides a way to convert a polygon down to triangles.
pub trait EmitTriangles {
    /// The content of each point in the triangle.
    type Vertex;

    /// Convert a polygon to one or more triangles, each triangle
    /// is returned by calling `emit`.
    fn emit_triangles<F>(&self, emit: F)
    where
        F: FnMut(Triangle<Self::Vertex>);
}

impl<T: Clone> EmitTriangles for Quad<T> {
    type Vertex = T;

    fn emit_triangles<F>(&self, mut emit: F)
    where
        F: FnMut(Triangle<T>),
    {
        let &Quad {
            ref x,
            ref y,
            ref z,
            ref w,
        } = self;
        emit(Triangle::new(x.clone(), y.clone(), z.clone()));
        emit(Triangle::new(z.clone(), w.clone(), x.clone()));
    }
}

impl<T: Clone> EmitTriangles for Triangle<T> {
    type Vertex = T;

    fn emit_triangles<F>(&self, mut emit: F)
    where
        F: FnMut(Triangle<T>),
    {
        emit(self.clone());
    }
}

impl<T: Clone> EmitTriangles for Polygon<T> {
    type Vertex = T;

    fn emit_triangles<F>(&self, emit: F)
    where
        F: FnMut(Triangle<T>),
    {
        match self {
            PolyTri(t) => t.emit_triangles(emit),
            PolyQuad(q) => q.emit_triangles(emit),
        }
    }
}

/// A trait to easily convert any polygon [`Iterator`] into a triangle [`Iterator`].
/// This is useful since Quads and other geometry are not supported by modern graphics pipelines like OpenGL.
pub trait Triangulate<T, V> {
    /// Convert an [`Iterator`] of polygons into an [`Iterator`] of triangles.
    fn triangulate(self) -> TriangulateIterator<T, V>;
}

impl<V, P: EmitTriangles<Vertex = V>, T: Iterator<Item = P>> Triangulate<T, V> for T {
    fn triangulate(self) -> TriangulateIterator<T, V> {
        TriangulateIterator::new(self)
    }
}

/// An [`Iterator`] that turns an [`Iterator`] of polygons into an [`Iterator`] of triangles.
///
/// This `struct` is created by the [`triangulate`] method on [`Triangulate`].
///
/// [`triangulate`]: trait.Triangulate.html#method.triangulate
/// [`Triangulate`]: trait.Triangulate.html
pub struct TriangulateIterator<SRC, V> {
    source: SRC,
    buffer: VecDeque<Triangle<V>>,
}

impl<V, U: EmitTriangles<Vertex = V>, SRC: Iterator<Item = U>> TriangulateIterator<SRC, V> {
    fn new(src: SRC) -> TriangulateIterator<SRC, V> {
        TriangulateIterator {
            source: src,
            buffer: VecDeque::new(),
        }
    }
}

impl<V, U: EmitTriangles<Vertex = V>, SRC: Iterator<Item = U>> Iterator
    for TriangulateIterator<SRC, V>
{
    type Item = Triangle<V>;

    fn size_hint(&self) -> (usize, Option<usize>) {
        let (n, _) = self.source.size_hint();
        (n, None)
    }

    fn next(&mut self) -> Option<Self::Item> {
        loop {
            if let v @ Some(_) = self.buffer.pop_front() {
                break v;
            }

            self.source
                .next()?
                .emit_triangles(|v| self.buffer.push_back(v));
        }
    }
}
