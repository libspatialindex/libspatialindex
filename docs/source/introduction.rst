.. _introduction:

:Author: Marios Hadjieleftheriou
:Contact: mhadji@gmail.com
:Revision: 1.9.3
:Date: 10/23/2019


The entire website is available as a single PDF at https://libspatialindex.org/libspatialindex.pdf

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------




Library Goals
------------------------------------------------------------------------------

The purpose of this library is to provide:
 1. An extensible framework that will support robust spatial indexing
    methods.
 2. Support for sophisticated spatial queries. Range, point location,
    nearest neighbor and k-nearest neighbor as well as parametric
    queries (defined by spatial constraints) should be easy to deploy and run.
 3. Easy to use interfaces for inserting, deleting and updating information.
 4. Wide variety of customization capabilities. Basic index and storage
    characteristics like the page size, node capacity, minimum fan-out,
    splitting algorithm, etc. should be easy to customize.
 5. Index persistence. Internal memory and external memory structures
    should be supported.  Clustered and non-clustered indices should
    be easy to be persisted.

Features
------------------------------------------------------------------------------

* Generic main memory and disk based storage managers.
* R\*-tree index (also supports linear and quadratic splitting).
* MVR-tree index (a.k.a. PPR-tree).
* TPR-tree index.
* Advanced query capabilities, using Strategy and Visitor patterns.
* Arbitrary shaped range queries, by defining generic geometry interfaces.
* Large parameterization capabilities, including dimensionality, fill factor,
  node capacity, etc.
* STR packing / bulk loading.

Warnings
------------------------------------------------------------------------------

* The library is not thread-safe, even for seemingly read-only operations. Queries and updates must be run from within mutexes.
