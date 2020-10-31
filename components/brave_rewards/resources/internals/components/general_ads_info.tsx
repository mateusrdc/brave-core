/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

// Components
import { Button } from 'brave-ui/components'
import { ButtonWrapper } from '../style'

import { getLocale } from '../../../../common/locale'

interface Props {
  data: RewardsInternals.State
  onGet: () => void
}

export class GeneralAdsInfo extends React.Component<Props, {}> {
  render () {
    return (
      <>
        <ButtonWrapper>
          <Button
            text={getLocale('refreshButton')}
            size={'medium'}
            type={'accent'}
            onClick={this.props.onGet}
          />
        </ButtonWrapper>
      </>
    )
  }
}
