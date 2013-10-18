package com.randomtest;

// Import required java libraries
import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;
import java.util.*;
import java.net.*;

// Extend HttpServlet class
public class RandomTestErrorHandler extends HttpServlet
{
    private String RANDOMTEST_URL;

    public void init(ServletConfig servletConfig) throws ServletException{
        RANDOMTEST_URL = servletConfig.getInitParameter("RANDOMTEST_URL");
        if (RANDOMTEST_URL == null) {
            throw new ServletException("Missing RANDOMTEST_URL servlet parameter");
        }
    }

    // Method to handle GET method request.
    public void doGet (HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        
        assert RANDOMTEST_URL != null;

        // Analyze the servlet exception             
        Throwable throwable = (Throwable)
            request.getAttribute ("javax.servlet.error.exception");
        Integer statusCode = (Integer)
            request.getAttribute ("javax.servlet.error.status_code");
        String servletName = (String)
            request.getAttribute ("javax.servlet.error.servlet_name");
        if (servletName == null) {
            servletName = "Unknown";
        }
        String requestUri = (String) request.getAttribute ("javax.servlet.error.request_uri");
        if (requestUri == null) {
            requestUri = "Unknown";
        }

        // Set response content type
        response.setContentType ("text/plain");

        if (throwable != null) {

            StringWriter str = new StringWriter();
            throwable.printStackTrace(new PrintWriter(str));

            URL url = new URL(RANDOMTEST_URL);
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setDoOutput(true); 
            connection.setDoInput(true); 
            connection.setRequestMethod("POST"); 
            connection.setRequestProperty("Content-Type", "application/x-www-form-urlencoded"); 
            connection.setUseCaches(false);

            DataOutputStream wr = new DataOutputStream(connection.getOutputStream());
            wr.writeBytes("stacktrace=" + URLEncoder.encode(str.toString()));
            wr.flush();

            String line;
            BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));
            while ((line = reader.readLine()) != null) {
                System.out.println(line);
            }

            wr.close();
            connection.disconnect();


            PrintWriter out = response.getWriter ();
            out.println ("Error information");
            out.println ("RANDOMTEST_URL = " + RANDOMTEST_URL);
            out.println ("Servlet Name : " + servletName + "</br></br>");
            out.println ("Exception Type : " +
                         throwable.getClass ().getName () + "</br></br>");
            out.println ("The request URI: " + requestUri + "<br><br>");
            out.println ("The exception message: " + throwable.getMessage ());
            throwable.printStackTrace(out);

        }
    }
    // Method to handle POST method request.
    public void doPost (HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        doGet (request, response);
    }
}
