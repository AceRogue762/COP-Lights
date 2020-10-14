/**
 * Component to render the developer page, containing dev and
 * testing-specific actions and information.
 * 
 * Available developer functionality:
 * - Reset the board (/api/reset)
 */

import React, { Component } from "react";
import { Button } from '@material-ui/core';

class DeveloperPage extends Component {
  // Send GET request to API to reset the board
  reset() {
    fetch('/api/reset');
  }

  render() {
    return (
      <div>
        <Button 
          color="primary" 
          onClick={() => { this.reset() }}
        >
          Reset
        </Button>
      </div>
    );
  }
}
  
export default DeveloperPage;