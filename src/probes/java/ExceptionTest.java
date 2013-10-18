package com.randomtest.test;

// Import required java libraries
import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;
import java.util.*;

// Extend HttpServlet class
public class ExceptionTest extends HttpServlet
{
    // Method to handle GET method request.
    public void doGet (HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        throw new ServletException("Exception details");
    }
}


